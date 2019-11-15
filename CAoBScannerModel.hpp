#pragma once

enum OFFSET_TYPE
{
	OFFSET_NORMAL = 0,
	OFFSET_CALL,
	OFFSET_PTR1,
	OFFSET_PTR4
};

struct ITEM
{
public:
	ITEM();
	
	// disable copy constructor pls
	ITEM(const ITEM&) = delete;

	unsigned int id;

	QString name, pattern;
	int type, order, offset;

	// for background color
	bool searched;
	unsigned long long result;

public:
	void serialize(QSettings &settings) const;
	void deserialize(QSettings &settings);

public:
	static int s_id;
};

class CAoBScannerModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit CAoBScannerModel(QObject *parent = nullptr);
	~CAoBScannerModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
	bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
		const QModelIndex &destinationParent, int destinationChild) Q_DECL_OVERRIDE;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

public:
	void updateResult(unsigned int id, unsigned long long result);

	void read(QSettings &settings);
	void write(QSettings &settings);

private:
	QList<QSharedPointer<ITEM>> m_items;
};