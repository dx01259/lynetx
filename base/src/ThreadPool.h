/*
 * ThreadPool.h
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#ifndef BASE_SRC_THREADPOOL_H_
#define BASE_SRC_THREADPOOL_H_

#include"TMap.h"
#include"ThreadObject.h"
#include"MsgDeque.h"
#include<pthread.h>

#include<deque>

namespace lynetx {

/********************************************************
 **** 线程池，主要是用来调度线程，给线程分配任务。   ****
 ********************************************************/

class CThreadPool
{
public:
	CThreadPool();
	virtual ~CThreadPool();
public:
	bool CreateThreadPool(const size_t numbers);
	bool AddTask(const pthread_t thread, const pthread_func func, const void *args);
	void RemoveTask(const CThreadObject &object);
	void RemoveThreadObject(pthread_t thread);
	void ClearThreadPool();
	pthread_t GetBestThread(SCHEDULING_ALGORITHM type);
	void SafeIncreaseThreadNumbers()
	{
		AUTO_GUARD(g, THREAD_MUTEX, m_mutex);
		++m_ThreadNumbers;
	}
	void SafeDecreaseThreadNumbers()
	{
		AUTO_GUARD(g, THREAD_MUTEX, m_mutex);
		--m_ThreadNumbers;
	}
protected:
	friend pthread_t schedulingFCFS(CThreadPool &pool);//先到先服务优先调度算法
	friend pthread_t schedulingHRP(CThreadPool &pool); //高响应比优先调度算法
private:
	size_t			m_ThreadNumbers;	//线程句柄个数
	CMutex			m_mutex;				//线程之间的同步
	CMsgDeque<pthread_t>	m_Threads;//线程池中的线程ID
	CTMap<pthread_t, boost::shared_ptr<CThreadObject> > m_ThreadPool;
};

} /* namespace lynetx */

#endif /* BASE_SRC_THREADPOOL_H_ */
