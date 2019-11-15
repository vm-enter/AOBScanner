#include "stdafx.h"

#include "CAoBScannerModel.hpp"

QString get_type_string(int type)
{
	switch (type)
	{
		case OFFSET_NORMAL: return "NORMAL";
		case OFFSET_CALL: return "CALL";
		case OFFSET_PTR1: return "PTR1";
		case OFFSET_PTR4: return "PTR4";
	}
	return QString();
}

// ITEM
int ITEM::s_id = 0;
ITEM::ITEM()
{
	this->id = s_id++;
	this->type = OFFSET_NORMAL;
	this->order = 0;
	this->offset = 0;
	this->result = 0;
	this->searched = false;
}
void ITEM::serialize(QSettings &settings) const
{
	settings.setValue("name", this->name);
	settings.setValue("type", this->type);
	settings.setValue("order", this->order);
	settings.setValue("offset", this->offset);
	settings.setValue("pattern", this->pattern);
}
void ITEM::deserialize(QSettings &settings)
{
	this->name = settings.value("name").toString();
	this->pattern = settings.value("pattern").toString();
	this->type = settings.value("type").toInt();
	this->order = settings.value("order").toInt();
	this->offset = settings.value("offset").toInt();
	this->result = 0;
	this->searched = false;
}

// CAoBScannerModel
CAoBScannerModel::CAoBScannerModel(QObject *parent) : QAbstractTableModel(parent)
{
}
CAoBScannerModel::~CAoBScannerModel()
{
}

int CAoBScannerModel::rowCount(const QModelIndex &parent) const
{
	return this->m_items.size();
}
int CAoBScannerModel::columnCount(const QModelIndex &parent) const
{
	return 6;
}

QVariant CAoBScannerModel::data(const QModelIndex &index, int role) const
{
	const int row = index.row();
	if (this->m_items.size() <= row)
		return QVariant();

	const auto &item = this->m_items[row];
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch (index.column())
		{
			case 0: return item->name;
			case 1: return get_type_string(item->type);
			case 2: return item->order;
			case 3: return item->offset;
			case 4: return item->pattern;

				// x86 only?????????ASDFASFASFASFSADFASFDASDFASDF
			case 5: return QString("0x%1").arg(QString::number(item->result, 16).rightJustified(8, '0').toUpper());
		}
	}
	else if (role == Qt::BackgroundColorRole)
	{
		if (item->searched && item->result == 0)
			return QColor(Qt::red);
		else
			return QColor(Qt::transparent);
	}
	return QVariant();
}
bool CAoBScannerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	const int row = index.row();
	if (this->m_items.size() <= row)
		return false;

	if (role == Qt::EditRole)
	{
		auto &item = this->m_items[row];
		switch (index.column())
		{
			case 0:
			{
				item->name = value.toString();
				emit this->dataChanged(index, index);
				return true;
			}
			case 1:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok)
					return false;

				item->type = i;
				emit this->dataChanged(index, index);
				return true;
			}
			case 2:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok)
					return false;

				item->order = i;
				emit this->dataChanged(index, index);
				return true;
			}
			case 3:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok)
					return false;

				item->offset = i;
				emit this->dataChanged(index, index);
				return true;
			}
			case 4:
			{
				item->pattern = value.toString();
				emit this->dataChanged(index, index);
				return true;
			}
			case 5:
			{
				bool ok;
				qulonglong i = value.toULongLong(&ok);
				if (!ok)
					return false;

				item->result = i;
				item->searched = true;

				// update entire row
				emit this->dataChanged(this->index(row, 0), this->index(row, this->columnCount() - 1));
				return true;
			}
		}
	}

	return false;
}

QVariant CAoBScannerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
			case 0: return QString("Name");
			case 1: return QString("Type");
			case 2: return QString("Order");
			case 3: return QString("Offset");
			case 4: return QString("AoB");
			case 5: return QString("Result");
		}
	}
	else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
	{
		return QString::number(section);
	}

	return QVariant();
}

bool CAoBScannerModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (count < 0 || row < 0 || this->rowCount() < row)
		return false;

	if (row == 0)
	{
		// If row is 0, the rows are prepended to any existing rows in the parent.
		this->beginInsertRows(QModelIndex(), 0, count - 1);
		for (int i = 0; i < count; i++)
			this->m_items.prepend(QSharedPointer<ITEM>::create());
		this->endInsertRows();
	}
	else if (row == this->rowCount())
	{
		// If row is rowCount(), the rows are appended to any existing rows in the parent.
		this->beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount() + count - 1);
		for (int i = 0; i < count; i++)
			this->m_items.append(QSharedPointer<ITEM>::create());
		this->endInsertRows();
	}
	else
	{
		// Insert
		this->beginInsertRows(QModelIndex(), row, row + count - 1);
		for (int i = 0; i < count; i++)
			this->m_items.insert(row, QSharedPointer<ITEM>::create());
		this->endInsertRows();
	}

	return true;
}
bool CAoBScannerModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (count < 1)
		return false;

	if ((row + count) > this->m_items.size())
		return false;

	// モデルからデータを削除する前に呼び出す。
	this->beginRemoveRows(parent, row, row + count - 1);

	// 実際のデータから削除
	this->m_items.erase(this->m_items.begin() + row, this->m_items.begin() + row + count);

	// モデルからデータを削除した後に呼び出す。
	this->endRemoveRows();

	return true;
}

bool CAoBScannerModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
	const QModelIndex &destinationParent, int destinationChild)
{
	// On models that support this, moves count rows starting with the given sourceRow 
	// under parent sourceParent to row destinationChild under parent destinationParent.
	if (count < 1)
		return false;

	if ((sourceRow + count) > this->m_items.size())
		return false;

	if (destinationChild < 0 || destinationChild > this->m_items.size())
		return false;

	if (sourceRow == destinationChild)
		return true;

	this->beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

	if (destinationChild < sourceRow)
	{
		// 下から上
		QList<QSharedPointer<ITEM>> _moveRows;
		for (int i = 0; i < count; i++)
			_moveRows.push_front(this->m_items.takeAt(sourceRow + i));

		foreach(const auto &row, _moveRows)
			this->m_items.insert(destinationChild, row);
	}
	else
	{
		// 上から下
		for (int i = 0; i < count; i++)
			this->m_items.insert(destinationChild, this->m_items.at(sourceRow + count - i - 1));

		for (int i = 0; i < count; i++)
			this->m_items.removeAt(sourceRow);
	}

	this->endMoveRows();

	// Change header data
	emit this->headerDataChanged(Qt::Vertical, sourceRow, sourceRow + count - 1);

	return true;
}

QModelIndex CAoBScannerModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!this->hasIndex(row, column, parent))
		return QModelIndex();

	return this->createIndex(row, column, this->m_items[row].data());
}
Qt::ItemFlags CAoBScannerModel::flags(const QModelIndex &index) const
{
	// don't allow to edit column5(result)
	auto flags = QAbstractTableModel::flags(index);
	if (index.column() != 5)
		flags |= Qt::ItemIsEditable;
	return flags;
}

void CAoBScannerModel::updateResult(unsigned int id, unsigned long long result)
{
	const int size = this->m_items.size();
	for (int i = 0; i < size; i++)
	{
		auto& item = this->m_items[i];
		if (item->id == id)
		{
			this->setData(this->index(i, 5), result);
			return;
		}
	}
}

void CAoBScannerModel::read(QSettings &settings)
{
	this->beginResetModel();
	this->m_items.clear();

	const int size = settings.beginReadArray("items");
	for (int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);

		auto item = QSharedPointer<ITEM>::create();
		item->deserialize(settings);
		this->m_items.append(item);
	}
	settings.endArray();

	this->endResetModel();
}
void CAoBScannerModel::write(QSettings &settings)
{
	settings.beginWriteArray("items");
	const int size = this->m_items.size();
	for (int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);
		const auto& item = this->m_items[i];
		item->serialize(settings);
	}
	settings.endArray();
}