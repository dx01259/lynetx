/*
 * LocalIpList.cpp
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#include"LocalIpList.h"
#include<unistd.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>

#include<algorithm>
using namespace std;

namespace lynetx {

namespace net {

CLocalIpList::CLocalIpList()
{
	// TODO Auto-generated constructor stub

}

CLocalIpList::~CLocalIpList()
{
	// TODO Auto-generated destructor stub
}

int CLocalIpList::GetLocalIpList()
{
	this->m_iplist.clear();
	char name[128];
	if(-1 == gethostname(name, sizeof(name)))
	{
		return -1;
	}

	struct hostent *phost = NULL;
	phost = gethostbyname(name);
	if(NULL == phost)
	{
		return -2;
	}

	char **p = phost->h_addr_list;
	for(; *p!=NULL; ++p)
	{
		char str[16];
		inet_ntop(phost->h_addrtype, *p, str, sizeof(str)); //网络二进制--点分十进制 ntop比较新
		this->m_iplist.push_back(str);

		/*in_addr addr;
		memcpy(&addr.S_un.S_addr, *p, phost->h_length);
		char *v4ip = ::inet_ntoa(addr);
		string strip(v4ip);*/
		//this->m_iplist.push_back(strip);
	}

	return (int)m_iplist.size();
}

int CLocalIpList::GetLocalIpList(vector<string> &ips)
{
	this->GetLocalIpList();
	ips = this->m_iplist;
	return (int)ips.size();
}

bool CLocalIpList::islocalIp(const char *ip)
{
	if(memcmp(ip, "127.0.0.1", sizeof("127.0.0.1")) == 0)
		return true;
	vector<string>::iterator it = find(this->m_iplist.begin(), this->m_iplist.end(), ip);
	if(it != this->m_iplist.end())
		return true;
	return false;
}

} /* namespace net */

} /* namespace lynetx */
