/*
 * TcpConnectionPool.h
 *
 *  Created on: 2015年9月12日
 *      Author: dengxu
 */

#ifndef NET_SRC_TCPCONNECTIONPOOL_H_
#define NET_SRC_TCPCONNECTIONPOOL_H_

#include"TcpConnection.h"
#include"InetConnectionPool.h"
#include"../../base/src/TMap.h"
#include<boost/scoped_ptr.hpp>

namespace lynetx {

namespace net {

class CTcpConnectionPool:
		public CInetConnectionPool
{
public:
	CTcpConnectionPool(){}
	virtual ~CTcpConnectionPool(){}
public:
	static CTcpConnectionPool *Instance()
	{
		static CTcpConnectionPool connPool;
		return &connPool;
	}
public:
	CTMap<string, TcpConnectionPtr> *GetMapTcpConnection(){return &m_TcpConnection;}
protected:
	CTMap<string, TcpConnectionPtr> m_TcpConnection;//客户端所有连接套接字: host->connection
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_TCPCONNECTIONPOOL_H_ */
