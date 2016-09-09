/*
 * InetConnection.h
 *
 *  Created on: 2015年9月13日
 *      Author: dengxu
 */

#ifndef BASE_SRC_INETCONNECTION_H_
#define BASE_SRC_INETCONNECTION_H_

#include"InetAddr.h"
#include"../../base/src/IOBufferPtr.h"
#include"../../base/src/Timeout.h"
#include<boost/shared_ptr.hpp>

namespace lynetx {

namespace net {

class CInetConnection;

typedef boost::shared_ptr<CInetConnection> InetConnectionPtr;

class CInetConnection
{
public:
	CInetConnection():
		m_clientfd(-1),
		m_timeout(180),
		m_inetAddr(8000, "127.0.0.1")
	{}
	virtual ~CInetConnection(){}
public:
	void SetSocket(const int fd, const char *ip, const u_short port)
	{
		this->m_clientfd = fd;
		m_inetAddr = CInetAddr(port, ip);
	}

	void SetSocket(const int fd, const sockaddr_in &addr)
	{
		this->m_clientfd = fd;
		m_inetAddr = CInetAddr(addr);
	}
	CTimeout &GetTimeoutObject(){return m_timeout;}

	int GetClientfd(){return m_clientfd;}

	CInetAddr &GetInetAddrObject(){return m_inetAddr;}
public:
	virtual ssize_t Read(void *buff, const size_t buffsize)
	{
		return 0;
	}

	virtual ssize_t Write(const void *buff, const size_t buffsize)
	{
		return 0;
	}

	virtual ssize_t Read(CIOBufferPtr &buffer)
	{
		return 0;
	}

	virtual ssize_t Write(CIOBufferPtr &buffer)
	{
		return 0;
	}
protected:
	int 			m_clientfd;		//套接字ID
	CTimeout		m_timeout;		//套接字超时设置
	CInetAddr		m_inetAddr;		//Socket对应的地址信息
};

} /* namespace net */

} /* namespace lynetx */

#endif /* BASE_SRC_INETCONNECTION_H_ */
