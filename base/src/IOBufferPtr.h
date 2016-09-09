/*
 * IOBufferPtr.h
 *
 *  Created on: 2015年9月5日
 *      Author: dengxu
 */

#ifndef BASE_SRC_IOBUFFERPTR_H_
#define BASE_SRC_IOBUFFERPTR_H_

#include"Mutex.h"
#include"datatype.h"
#include"../../base/src/commfunc.h"

#include<string>
#include<vector>
using namespace std;

namespace lynetx {

struct mmset
{
	int rd_pos;
	int wr_pos;
	vector<byte> base;
};

class CIOBufferPtr
{
public:
	CIOBufferPtr(size_t size=1024/*1MB的缓存*/);
	virtual ~CIOBufferPtr();
public:
	void Resize(int size);
	int GetSize();
	int GetLength();
	int ReadableBytes(){return this->GetLength();}
	int WriteableBytes(){return GetSize()-m_mmset.wr_pos;}
	void Swap(CIOBufferPtr &buf)
	{
		AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

		this->m_mmset.base.swap(buf.m_mmset.base);
		std::swap(this->m_mmset.rd_pos, buf.m_mmset.rd_pos);
		std::swap(this->m_mmset.wr_pos, buf.m_mmset.wr_pos);
	}
	byte *ReadPosition()
	{
		AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);
		return &m_mmset.base[m_mmset.rd_pos];
	}
	byte *WritePosition()
	{
		AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);
		return &m_mmset.base[m_mmset.wr_pos];
	}
public:
	//skip data
	uint32 skip(size_t len)
	{
		mmset &mset = m_mmset;
		uint32 readlen = mset.base.size() - mset.wr_pos;
		uint32 minlen = d_min(readlen, len);

		mset.wr_pos += minlen;

		return minlen;
	}
	uint32 skipAll()
	{
		mmset &mset = m_mmset;
		uint32 readlen = mset.wr_pos - mset.rd_pos;

		mset.wr_pos += readlen;

		return readlen;
	}
	void Reset(){this->Clear();}

	//read data
	int Read(int8 &value);
	int Read(int16 &value);
	int Read(int32 &value);
	int Read(int64 &value);
	int Read(char &value);
	int Read(string &value, int len);

	//write data
	int Write(const int8 value);
	int Write(const int16 value);
	int Write(const int32 value);
	int Write(const int64 value);
	int Write(const char value);
	int Write(const string value);

	void Clear();
private:
	struct mmset m_mmset;
	CMutex m_mutex;
};

} /* namespace lynetx */

#endif /* BASE_SRC_IOBUFFERPTR_H_ */
