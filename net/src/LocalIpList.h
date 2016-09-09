/*
 * LocalIpList.h
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#ifndef NET_SRC_LOCALIPLIST_H_
#define NET_SRC_LOCALIPLIST_H_

#include<string>
#include<vector>
using namespace std;

namespace lynetx {

namespace net {

class CLocalIpList
{
public:
	CLocalIpList();
	virtual ~CLocalIpList();
public:
	int GetLocalIpList();
	int GetLocalIpList(vector<string> &ips);
	bool islocalIp(const char *ip);
private:
	vector<string> m_iplist;
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_LOCALIPLIST_H_ */
