#include "stdafx.h"

#include "CMainWindow.hpp"
#include "CComboBoxDelegate.hpp"
#include "CProcessDialog.hpp"
#include "CIntDelegate.hpp"
#include "CFindPatternWorker.hpp"
#include "CAoBScannerModel.hpp"

#include <winternl.h>
#pragma comment(lib, "ntdll.lib")

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent)
{
	this->setWindowTitle("Scanner");
	this->setMinimumSize(600, 400);

	// menu
	QMenu *fileMenu = new QMenu(tr("&File"), this);
	QMenu *toolsMenu = new QMenu(tr("&Tools"), this);
	QMenu *helpMenu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(toolsMenu);
	menuBar()->addMenu(helpMenu);

	// add actions
	fileMenu->addAction(QIcon(":/images/Computer-64.png"), tr("&Open Process"), this, SLOT(OnOpenProcess()));
	fileMenu->addSeparator();
	fileMenu->addAction(QIcon::fromTheme("file-open", QIcon(":/images/Open Folder-64.png")), tr("&Load"), this, SLOT(OnLoad()), QKeySequence::Open);
	fileMenu->addAction(QIcon::fromTheme("file-save", QIcon(":/images/Save-64.png")), tr("&Save"), this, SLOT(OnSave()), QKeySequence::Save);
	fileMenu->addAction(QIcon::fromTheme("file-saveas", QIcon(":/images/Save as-64.png")), tr("&Save As..."), this, SLOT(OnSaveAs()), QKeySequence::SaveAs);
	fileMenu->addSeparator();
	fileMenu->addAction(QIcon::fromTheme("file-quit", QIcon(":/images/Close Window-64.png")), tr("&Quit"), this, SLOT(close()), Qt::CTRL + Qt::Key_Q);

	toolsMenu->addAction(QIcon(), tr("&Export To File"), this, SLOT(OnExport()));
	
	helpMenu->addAction(QIcon::fromTheme("help-about", QIcon(":/images/About-64.png")), tr("&About"), this, SLOT(about()));
	helpMenu->addAction(style()->standardIcon(QStyle::SP_TitleBarMenuButton), tr("About &Qt"), qApp, SLOT(aboutQt()));

	// main widget
	QList<QPair<QString, QVariant>> items;
	items << QPair<QString, QVariant>(tr("NORMAL"), OFFSET_NORMAL)
		<< QPair<QString, QVariant>(tr("CALL"), OFFSET_CALL)
		<< QPair<QString, QVariant>(tr("PTR1"), OFFSET_PTR1)
		<< QPair<QString, QVariant>(tr("PTR4"), OFFSET_PTR4);

	this->m_model = new CAoBScannerModel(this);

	this->m_view = new QTableView(this);
	this->m_view->setModel(this->m_model);
	this->m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->m_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	this->m_view->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	// delegates
	this->m_view->setItemDelegateForColumn(1, new CComboBoxDelegate(std::move(items), this)); // doesn't have rvalue impl tho
	this->m_view->setItemDelegateForColumn(2, new CIntDelegate(1, 100, this));
	this->m_view->setItemDelegateForColumn(3, new CIntDelegate(-1000, 1000, this));

	// movable
	this->m_view->verticalHeader()->setSectionsMovable(true);
	this->m_view->verticalHeader()->setDragEnabled(true);
	this->m_view->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
	this->m_view->verticalHeader()->setDropIndicatorShown(true);
	this->m_view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		
	//
	this->setCentralWidget(this->m_view);
	this->m_sFilename = QString();
	this->m_moduleBuffer = NULL;

	QObject::connect(this->m_view, &QTableView::customContextMenuRequested, this, &CMainWindow::OnContextMenuRequested);

	// load settings
	QSettings settings;

	// window settings
	QVariant size = settings.value("windowsize");
	if (size.isValid())
		this->resize(size.toSize());

	QVariant pos = settings.value("windowpos");
	if (pos.isValid())
		this->move(pos.toPoint());

	// table view settings
	for (int i = 0; i < this->m_view->horizontalHeader()->count(); i++)
	{
		QVariant width = settings.value(QString("tableview/column/%1/width").arg(i));
		if (width.isValid())
			this->m_view->setColumnWidth(i, width.toInt());
	}

	// 8 threads perhaps?
	qRegisterMetaType<FindPatternParam>("FindPatternParam");
	for (int i = 0; i < 8; i++)
	{
		QThread *workerThread = new QThread(this);
		CFindPatternWorker *worker = new CFindPatternWorker;
		worker->moveToThread(workerThread);
		QObject::connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
		QObject::connect(worker, &CFindPatternWorker::resultReady, this, &CMainWindow::handleResults);
		workerThread->start();

		struct Worker workerStruct;
		workerStruct.thread = workerThread;
		workerStruct.worker = worker;
		workerStruct.requestCount = 0;
		this->m_workers.append(workerStruct);
	}
}
CMainWindow::~CMainWindow()
{
	for (auto a : this->m_workers)
	{
		a.thread->quit();
	}

	for (auto a : this->m_workers)
	{
		a.thread->wait();
	}
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
	QSettings settings;
	settings.setValue("filename", this->m_sFilename);
	settings.setValue("windowsize", this->size());
	settings.setValue("windowpos", this->pos());
	for (int i = 0; i < this->m_view->horizontalHeader()->count(); i++)
		settings.setValue(QString("tableview/column/%1/width").arg(i), this->m_view->columnWidth(i));

	if (this->m_moduleBuffer)
		VirtualFree(this->m_moduleBuffer, 0, MEM_RELEASE);

	event->accept();
}

void CMainWindow::OnContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = this->m_view->indexAt(pos);
	QMenu *menu = new QMenu(this);
	QAction *action;

	// scan
	action = menu->addAction("Scan", this, SLOT(OnScan()));
	action->setData(index);
	action->setEnabled(index.isValid());
	menu->addAction(action);

	// scan all and separator :^)
	menu->addAction(QIcon(":/images/Search-64.png"), "Scan All", this, SLOT(OnScanAll()));
	menu->addSeparator();

	// insert row
	action = menu->addAction(QIcon(":/images/Add Row-64.png"), "Insert Row", this, SLOT(OnInsertRow()));
	action->setData(index);
	// action->setEnabled(index.isValid());
	menu->addAction(action);

	// remove row
	action = menu->addAction(QIcon(":/images/Delete Row-64.png"), "Remove Row", this, SLOT(OnRemoveRow()));
	action->setData(index);
	action->setEnabled(index.isValid());

	menu->popup(this->m_view->viewport()->mapToGlobal(pos));
}

void CMainWindow::about()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(tr("About"));
	msgBox.setText(
		"<b>Scanner</b><br>" \
		"<br>" \
		"Special Thanks : <a href='https://icons8.com/#'>icons8</a> for free icons.");
	msgBox.setTextFormat(Qt::RichText);
	msgBox.exec();
}

void CMainWindow::OnLoad()
{
	const QString filename = QFileDialog::getOpenFileName(this, QString(), QSettings().value("inifile").toString(), "Ini files (*.ini)");
	if (filename.isEmpty())
		return;

	this->m_sFilename = filename;
	QSettings settings(this->m_sFilename, QSettings::IniFormat);
	this->m_model->read(settings);
}
void CMainWindow::OnSave()
{
	if (this->m_sFilename.isEmpty())
	{
		// if no file exists, try to save as...
		this->OnSaveAs();
		return;
	}

	QSettings settings(this->m_sFilename, QSettings::IniFormat);
	settings.clear();
	this->m_model->write(settings);

	QMessageBox::information(this, QString(), tr("Done."), QMessageBox::Ok);
}
void CMainWindow::OnSaveAs()
{
	const QString filename = QFileDialog::getSaveFileName(this, QString(), QSettings().value("inifile").toString(), "Ini files (*.ini)");
	if (filename.isEmpty())
		return;

	this->m_sFilename = filename;
	this->OnSave();
}

BOOL GetProcessModuleInfo(HANDLE hProcess, LPVOID *lplpBaseOfDll, LPDWORD lpdwSizeOfImage)
{
	if (lplpBaseOfDll == NULL || lpdwSizeOfImage == NULL)
		return FALSE;

	PROCESS_BASIC_INFORMATION pbi;
	if (NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL)))
	{
		PEB peb;
		if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(PEB), NULL))
			return FALSE;

		// process module
		HMODULE hModule = (HMODULE)peb.Reserved3[1];

		// manual
		IMAGE_DOS_HEADER imageDosHeader;
		IMAGE_NT_HEADERS imageNtHeaders;
		LONG e_lfanew;

		// read dos header
		if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(hModule), &imageDosHeader, sizeof(IMAGE_DOS_HEADER), NULL))
			return FALSE;

		if (imageDosHeader.e_magic != IMAGE_DOS_SIGNATURE)
		{
			// wrong signature
			return FALSE;
		}

		e_lfanew = imageDosHeader.e_lfanew;
		if (e_lfanew >= 0 && e_lfanew < 0x10000000)
		{
			if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(PBYTE(hModule) + e_lfanew),
				&imageNtHeaders, sizeof(IMAGE_NT_HEADERS), NULL))
			{
				return FALSE;
			}

			if (imageNtHeaders.Signature == IMAGE_NT_SIGNATURE)
			{
				*lplpBaseOfDll = hModule;
				*lpdwSizeOfImage = imageNtHeaders.OptionalHeader.SizeOfCode;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CMainWindow::OnOpenProcess()
{
	CProcessDialog processDialog;
	if (processDialog.exec() == QDialog::Rejected)
	{
		qDebug() << "Rejected";
		return;
	}

	DWORD dwProcessId = processDialog.GetSelectedProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		QMessageBox::information(this, QString(), QString("OpenProcess failed."), QMessageBox::Ok);
		return;
	}

	LPVOID lpBaseOfDll;
	DWORD dwSizeOfImage;
	if (!GetProcessModuleInfo(hProcess, &lpBaseOfDll, &dwSizeOfImage))
	{
		CloseHandle(hProcess);
		QMessageBox::information(this, QString(), QString("GetProcessModuleInfo failed."), QMessageBox::Ok);
		return;
	}

	if (this->m_moduleBuffer)
		VirtualFree(this->m_moduleBuffer, 0, MEM_RELEASE);

	this->m_moduleBuffer = VirtualAlloc(NULL, dwSizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	this->m_moduleBaseAddress = lpBaseOfDll;
	this->m_dwSizeOfImage = dwSizeOfImage;
	if (!this->m_moduleBuffer)
	{
		CloseHandle(hProcess);
		return;
	}

	if (!ReadProcessMemory(hProcess, this->m_moduleBaseAddress, this->m_moduleBuffer, this->m_dwSizeOfImage, NULL))
	{
		VirtualFree(this->m_moduleBuffer, 0, MEM_RELEASE);
		this->m_moduleBuffer = NULL;
		CloseHandle(hProcess);
		return;
	}

	CloseHandle(hProcess);

	this->setWindowTitle(QString("Scanner - %1").arg(QString::number(dwProcessId)));
}

void CMainWindow::OnExport()
{
	QSettings settings;
	QString filename = QFileDialog::getSaveFileName(this, QString(), settings.value("exportDir").toString(), "Header files (*.h;*.hpp;*.txt)");
	if (filename.isEmpty())
		return;

	settings.setValue("exportDir", filename);

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return;

	const QString ver = QInputDialog::getText(this, QString(), tr("Enter comment :"));
	if (ver.isEmpty())
		return;

	QTextStream stream(&file);
	for (int row = 0; row < this->m_model->rowCount(); row++)
	{
		const QString name = this->m_model->data(this->m_model->index(row, 0)).toString();
		if (name.isEmpty())
		{
			stream << tr("\r\n");
			continue;
		}

		const QString result = this->m_model->data(this->m_model->index(row, 5)).toString();
		stream << tr("#define %1 %2 // %3\r\n").arg(name).arg(result).arg(ver);
	}

	QMessageBox::information(this, QString(), tr("Done."), QMessageBox::Ok);
}

void CMainWindow::OnScan()
{
	const QModelIndex &index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	if (!index.isValid())
		return;

	if (this->m_moduleBuffer == NULL)
	{
		// open process if not
		this->OnOpenProcess();
		if (this->m_moduleBuffer == NULL)
			return;
	}

	const QString arrayOfBytes = this->m_model->index(index.row(), 4).data().toString();
	if (arrayOfBytes.isEmpty())
		return;

	this->Scan(index);
}
void CMainWindow::OnScanAll()
{
	if (this->m_moduleBuffer == NULL)
	{
		// open process if not
		this->OnOpenProcess();
		if (this->m_moduleBuffer == NULL)
			return;
	}

	this->ScanAll();
}

void CMainWindow::OnInsertRow()
{
	const QModelIndex &index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	if (!index.isValid())
	{
		this->m_model->insertRow(this->m_model->rowCount());
	}
	else
	{
		this->m_model->insertRow(index.row() + 1);
	}
}
void CMainWindow::OnRemoveRow()
{
	const QModelIndex &index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	if (!index.isValid())
		return;

	this->m_model->removeRow(index.row());
}

void CMainWindow::Scan(const QModelIndex &index)
{
	if (!index.isValid())
		return;

	// sort by request count
	qSort(this->m_workers.begin(), this->m_workers.end(), [](struct Worker &w1, struct Worker &w2) -> bool
	{
		return w1.requestCount < w2.requestCount;
	});

	struct ITEM *item = static_cast<ITEM *>(index.internalPointer());
	if (item->pattern.isEmpty())
	{
		// probably separater if no patterns
		return;
	}

	FindPatternParam param(item->id, (unsigned long long)this->m_moduleBaseAddress, this->m_moduleBuffer, this->m_dwSizeOfImage,
		item->pattern, item->type, item->offset, item->order);

	auto &worker = this->m_workers[0];
	QMetaObject::invokeMethod(worker.worker, "doWork", Qt::QueuedConnection, Q_ARG(const FindPatternParam&, param));
	worker.requestCount++;
}
void CMainWindow::ScanAll()
{
	for (int i = 0; i < this->m_model->rowCount(); i++)
		this->Scan(this->m_model->index(i, 0));
}

void CMainWindow::handleResults(unsigned int id, unsigned long long result)
{
	CFindPatternWorker *worker = qobject_cast<CFindPatternWorker *>(this->sender());
	for (auto &workerStruct : this->m_workers)
	{
		if (workerStruct.worker == worker)
		{
			workerStruct.requestCount = std::max<>(0, workerStruct.requestCount - 1);
			break;
		}
	}

	this->m_model->updateResult(id, result);
}