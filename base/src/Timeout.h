/*
 * Timeout.h
 *
 *  Created on: 2015年9月1日
 *      Author: dengxu
 */

#ifndef BASE_SRC_TIMEOUT_H_
#define BASE_SRC_TIMEOUT_H_

#include<sys/time.h>
#include"datatype.h"

namespace lynetx {


class CTimeout
{
public:
	CTimeout(const u_long to=0);
	virtual ~CTimeout();
public:
	void Init(u_long out);
	void Reset();
	u_long Start();
	void SetTimeout(u_long out);
	bool IsTimeout() const;
	u_long elapse() const;
	u_long timeleft();
	u_long timeGetTime() const;
	static bool timeofNowTime(struct timespec &spec, const u_long minisec);
private:
	bool   m_started;
	u_long m_startTime;
	u_long m_timeout;//设置超时的秒数
};

} /* namespace lynetx */

#endif /* BASE_SRC_TIMEOUT_H_ */
