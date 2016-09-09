/*
 * InetTable.h
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#ifndef NET_SRC_INETTABLE_H_
#define NET_SRC_INETTABLE_H_

#include"InetAddr.h"
#include"../../base/src/TMap.h"

namespace lynetx {

namespace net {

class CInetTable
{
public:
	CInetTable();
	virtual ~CInetTable();
public:
	int GetIpAddr(const string &szID, CInetAddr *addrp);
	void AddIpAddr(const string &szID,  const CInetAddr *addrp);
	bool RemoveIpAddr(const string &szID);
	void ClearIpAddr();
private:
	CTMap<string, CInetAddr *> m_iptables;
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_INETTABLE_H_ */
