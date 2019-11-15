#pragma once

QT_BEGIN_NAMESPACE
class QTableView;
class CAoBScannerModel;
class CFindPatternWorker;
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
	Q_OBJECT

	struct Worker
	{
		QThread *thread;
		CFindPatternWorker *worker;
		int requestCount;
	};

public:
	explicit CMainWindow(QWidget *parent = nullptr);
	~CMainWindow();

protected:
	void closeEvent(QCloseEvent *) Q_DECL_OVERRIDE;

private slots:
	void OnContextMenuRequested(const QPoint &pos);
	void about();
	void OnLoad();
	void OnSave();
	void OnSaveAs();
	void OnOpenProcess();
	void OnExport();
	void OnScan();
	void OnScanAll();
	void OnInsertRow();
	void OnRemoveRow();

private:
	void Scan(const QModelIndex &index);
	void ScanAll();

public slots:
	void handleResults(unsigned int id, unsigned long long result);

private:
	QTableView *m_view;
	CAoBScannerModel *m_model;
	QString m_sFilename;

	LPVOID m_moduleBuffer;
	LPVOID m_moduleBaseAddress;
	DWORD m_dwSizeOfImage;

	//
	QList<struct Worker> m_workers;
};