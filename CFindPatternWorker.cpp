#include "stdafx.h"

#include "CFindPatternWorker.hpp"
#include "findpattern.hpp"
#include "CAoBScannerModel.hpp"

CFindPatternWorker::CFindPatternWorker(QObject *parent) : QObject(parent)
{
}
CFindPatternWorker::~CFindPatternWorker()
{
}

void CFindPatternWorker::doWork(const FindPatternParam& param)
{
	// params
	const char *_AoB = param.arrayOfBytes().toStdString().c_str();
	const int order = param.order();

	const unsigned long long moduleBaseAddress = param.moduleBaseAddress();
	const unsigned long bufferSize = param.bufferSize();

	const void *bufferForLocal = param.moduleBuffer();
	const void *bufferEnd = reinterpret_cast<const char *>(bufferForLocal) + bufferSize;

	const void *searchBuffer = bufferForLocal;
	unsigned long remainingSize = bufferSize;
	void *resultBuffer = nullptr;
	for (int n = 0; n < order; n++)
	{
		// n result
		if (n != 0)
		{
			searchBuffer = PBYTE(searchBuffer) + 1;
			remainingSize = ULONG_PTR(bufferEnd) - ULONG_PTR(searchBuffer);
		}

		resultBuffer = FindPatternA(searchBuffer, remainingSize, _AoB);
		if (resultBuffer == NULL)
		{
			// fail
			emit this->resultReady(param.id(), 0);
			return;
		}
	}

	const unsigned long long delta = moduleBaseAddress - (unsigned long long)bufferForLocal;
	unsigned long long result = (unsigned long long)resultBuffer + param.offset();
	switch (param.type())
	{
		case OFFSET_NORMAL:
		{
			// fix it up
			result = result + delta;
			break;
		}
		case OFFSET_CALL:
		{
			// for x86 only?
			const int callOffset = *(unsigned long *)(result + 1);
			result = result + delta + callOffset + 5;
			break;
		}
		case OFFSET_PTR1:
		{
			void *ptr = reinterpret_cast<char *>(result);
			if (bufferForLocal <= ptr && ptr < bufferEnd)
			{
				result = *(unsigned char *)(ptr);
			}
			else
			{
				// invalid ptr
				result = 0;
			}
			break;
		}
		case OFFSET_PTR4:
		{
			void *ptr = reinterpret_cast<char *>(result);
			if (bufferForLocal <= ptr && ptr < bufferEnd)
			{
				result = *(unsigned long *)(ptr);
			}
			else
			{
				// invalid ptr
				result = 0;
			}
			break;
		}
		default:
		{
			result = 0;
			break;
		}
	}

	emit this->resultReady(param.id(), result);
}