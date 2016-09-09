/*
 * TcpConnection.h
 *
 *  Created on: 2015年9月5日
 *      Author: dengxu
 */

#ifndef NET_SRC_TCPCONNECTION_H_
#define NET_SRC_TCPCONNECTION_H_

#include"SocketOps.h"
#include"InetConnection.h"
#include"../../base/src/logdef.h"
#include<boost/shared_ptr.hpp>

namespace lynetx {

namespace net {

class CTcpConnection;

typedef boost::shared_ptr<CTcpConnection> TcpConnectionPtr;

class CTcpConnection :
		public CInetConnection
{
public:
	CTcpConnection(){}
	virtual ~CTcpConnection(){}
public:
	ssize_t Read(void *buff, const size_t buffsize)
	{
		ssize_t result = SocketOps::read(m_clientfd, buff, buffsize);
		if(result == -1)
		{
			if(errno!=EAGAIN &&
				errno!=EWOULDBLOCK &&
				errno!=EISDIR)
			{
				TRACE_ERR(LOG_ERRORS, errno, 100, "Read datas to socket %d happen error", m_clientfd);
				return result;
			}
			return -2;
		}
		return result;
	}

	ssize_t Write(const void *buff, const size_t buffsize)
	{
		ssize_t result = SocketOps::write(m_clientfd, buff, buffsize);
		if(-1 == result)
		{
			if(errno!=EAGAIN &&
				errno!=EWOULDBLOCK &&
				errno!=EISDIR)
			{
				TRACE_ERR(LOG_ERRORS, errno, 100, "Write datas to socket %d happen error", m_clientfd);
				return result;
			}
			return -2;
		}
		return result;
	}

	ssize_t Read(CIOBufferPtr &buffer)
	{
		int len = buffer.GetSize(), maxsize=1024*1024;
		if(len < maxsize)//1KB大小的空间
		{
			len = maxsize;
			buffer.Resize(len);
		}
		return Read(buffer.WritePosition(), len);
	}

	ssize_t Write(CIOBufferPtr &buffer)
	{
		ssize_t readableLen = buffer.ReadableBytes();
		ssize_t lentmp = readableLen;
		while(lentmp > 1024)
		{
			lentmp -= 1024;
			Write(static_cast<const void *>(buffer.ReadPosition()), lentmp);
		}
		Write(static_cast<const void *>(buffer.ReadPosition()), lentmp);

		return readableLen;
	}
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_TCPCONNECTION_H_ */
