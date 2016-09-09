/*
 * ThreadObject.h
 *
 *  Created on: 2015年9月1日
 *      Author: dengxu
 */

#ifndef BASE_SRC_THREADOBJECT_H_
#define BASE_SRC_THREADOBJECT_H_

#include"ThreadDefine.h"
#include<boost/shared_ptr.hpp>

namespace lynetx {

typedef boost::shared_ptr<thread_task> TaskPtr;

class CThreadObject {
public:
	CThreadObject();
	virtual ~CThreadObject();
public:
	void InitObject();
	bool CreateThreadObject();
	void ThreadObjectWaitquit();
	void CancleThreadObject(const size_t waitusecs=0);
	bool AddThreadTask(const pthread_func func, const void *args);
	void SetThreadStatus(const size_t busying, const thread_status status)
	{
		AUTO_GUARD(g, THREAD_MUTEX, m_mutex);
		m_thread.busying = busying;
		m_thread.status = status;
	}
	void GetThreadStatus(size_t &busying, thread_status &status)
	{
		AUTO_GUARD(g, THREAD_MUTEX, m_mutex);
		busying = m_thread.busying;
		status = m_thread.status;
	}
	inline void ReadyRunningPost(){sem_post(&m_ReadyRunning);}
	pthread_t GetThreadID(){return this->m_thread.thread;}
	size_t GetThreadBusying(){return this->m_thread.busying;}
protected:
	static void *thread_proc(void *arg);
	static void clean_func(void *arg);
private:
	sem_t		m_ReadyRunning;		//线程就绪队列和运行队列的缓存更新事件
	CMutex		m_mutex;				//线程之间的同步
	pthread_attr_t m_attr;			//线程的属性
	thread_struct m_thread;		//保存线程基本信息
	CMsgDeque<TaskPtr>	m_pTasks;	//线程对应的任务处理
};

} /* namespace lynetx */

#endif /* BASE_SRC_THREADOBJECT_H_ */
