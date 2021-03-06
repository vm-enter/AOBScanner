#pragma once

class FindPatternParam
{
public:
	FindPatternParam() {}
	FindPatternParam(unsigned int id, unsigned long long baseAddr, void *buffer, unsigned long size,
		const QString &aobString, int type, int offset, int order)
	{
		this->m_id = id;
		this->m_moduleBaseAddress = baseAddr;
		this->m_moduleBuffer = buffer;
		this->m_bufferSize = size;
		this->m_arrayOfBytes = aobString;
		this->m_type = type;
		this->m_offset = offset;
		this->m_order = order;
	}
	~FindPatternParam() {}

	unsigned int id() const { return this->m_id; }

	unsigned long long moduleBaseAddress() const { return this->m_moduleBaseAddress; }
	void *moduleBuffer() const { return this->m_moduleBuffer; }

	QString arrayOfBytes() const { return this->m_arrayOfBytes; }
	unsigned long bufferSize() const { return this->m_bufferSize; }

	int type() const { return this->m_type; }
	int offset() const { return this->m_offset; }
	int order() const { return this->m_order; }

private:
	// incase user removed item from view
	unsigned int m_id;

	// xD!
	unsigned long long m_moduleBaseAddress;		// HMODULE
	void *m_moduleBuffer;			// for local
	unsigned long m_bufferSize;		// from NTHeader

	// input
	QString m_arrayOfBytes;
	int m_type, m_offset, m_order;
};

Q_DECLARE_METATYPE(FindPatternParam);

class CFindPatternWorker : public QObject
{
	Q_OBJECT

public:
	CFindPatternWorker(QObject *parent = nullptr);
	~CFindPatternWorker();

public slots:
	void doWork(const FindPatternParam& param);

signals:
	void resultReady(unsigned int id, unsigned long long result);
};