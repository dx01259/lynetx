/*
 * InetConnectionPool.h
 *
 *  Created on: 2015年9月13日
 *      Author: dengxu
 */

#ifndef BASE_SRC_INETCONNECTIONPOOL_H_
#define BASE_SRC_INETCONNECTIONPOOL_H_

#include"InetConnection.h"
#include"../../base/src/TMap.h"

namespace lynetx {

namespace net {

class CInetConnectionPool
{
public:
	CInetConnectionPool(){}
	virtual ~CInetConnectionPool(){}
public:
	CTMap<int, std::string> *GetSocket2InetAddr(){return &m_Sockfd2InetAddr;}
protected:
	CTMap<int, string>	m_Sockfd2InetAddr;		//socket描述符到IP:PORT地址
};

} /* namespace net */

} /* namespace lynetx */

#endif /* BASE_SRC_INETCONNECTIONPOOL_H_ */
