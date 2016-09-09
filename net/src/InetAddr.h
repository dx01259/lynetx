/*
 * InetAddr.h
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#ifndef NET_SRC_INETADDR_H_
#define NET_SRC_INETADDR_H_

#include"../../base/src/datatype.h"
#include<netinet/in.h>
#include<arpa/inet.h>

#include<stdlib.h>
#include<string>
using namespace std;

namespace lynetx {

namespace net {

class CInetAddr
{
public:
	CInetAddr(u_short usPort, const char *addr);
	CInetAddr(const sockaddr_in &addr);
	virtual ~CInetAddr();
public:
	string GetIp() const{return inet_ntoa(m_addr.sin_addr);}
	u_short GetPort() const{return ntohs(m_addr.sin_port);}
	string toIpAndPort() const
	{
		string szIP, szIPAndPort, szTmp;
		szIP   = GetIp();
		szTmp.resize(szIP.size()+10);
		sprintf(&szTmp[0], "%s:%d", szIP.c_str(), GetPort());

		return szIPAndPort=szTmp;
	}
	void SetSockType(const int socktype);
	int  GetSockType() const;
	void GetInetAddr(sockaddr_in &addr);
private:
	sockaddr_in m_addr;
	int m_socktype;
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_INETADDR_H_ */
