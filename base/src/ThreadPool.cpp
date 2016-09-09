/*
 * ThreadPool.cpp
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#include"ThreadPool.h"
#include"logdef.h"
#include<boost/shared_ptr.hpp>
#include<boost/make_shared.hpp>

namespace lynetx {

//先到先服务优先调度算法
pthread_t schedulingFCFS(CThreadPool &pool)
{
	boost::shared_ptr<CThreadObject> val;
	pthread_t thread;
	if(!pool.m_Threads.PopFront(thread)){
		return 0;
	}
	if(pool.m_ThreadPool.find(thread, val) == 0)
	{
		pool.m_Threads.PushBack(thread);
		return thread;
	}
	else{
		TRACE_DUMP(LOG_ERRORS, 100, "获取线程ID发生异常");
	}
	return 0;
}

//高响应比优先调度算法
pthread_t schedulingHRP(CThreadPool &pool)
{
	pthread_t thread = -1;
	CThreadObject *bestThread = NULL;
	vector<boost::shared_ptr<CThreadObject> > vThreadObj;
	pool.m_ThreadPool.get_all_values(vThreadObj);
	if(vThreadObj.size() > 0)
	{
		size_t i = 0;
		bestThread = vThreadObj[0].get();
		for(; i<vThreadObj.size(); ++i)
		{
			CThreadObject *obj = vThreadObj[i].get();
			assert(obj!=NULL);
			if(obj->GetThreadBusying() <=0 )
			{
				thread = obj->GetThreadID();
				break;
			}
			else if(bestThread->GetThreadBusying() > obj->GetThreadBusying())
			{
				bestThread = obj;
			}
		}
		if(i == vThreadObj.size())
		{
			thread = bestThread->GetThreadID();
		}
	}

	return thread;
}

CThreadPool::CThreadPool() :
		m_ThreadNumbers(0)
{

}

CThreadPool::~CThreadPool()
{

}

bool CThreadPool::CreateThreadPool(const size_t numbers)
{
	for(size_t i=0; i<numbers; ++i)
	{
		boost::shared_ptr<CThreadObject> pobject = boost::make_shared<CThreadObject>();
		if(pobject->CreateThreadObject())
		{
			SafeIncreaseThreadNumbers();
			m_ThreadPool.set(pobject.get()->GetThreadID(), pobject);
			m_Threads.PushBack(pobject.get()->GetThreadID());
		}
	}
	return true;
}

bool CThreadPool::AddTask(const pthread_t thread, const pthread_func func, const void *args)
{
	boost::shared_ptr<CThreadObject> obj;
	if(m_ThreadPool.find(thread, obj) == 0)
	{
		obj.get()->AddThreadTask(func, args);
	}
	else{
		TRACE_MSG(LOG_ERRORS, 100, "线程池中没有匹配的线程");
		return false;
	}
	return true;
}

void CThreadPool::RemoveTask(const CThreadObject &object)
{

}

void CThreadPool::RemoveThreadObject(pthread_t thread)
{
	boost::shared_ptr<CThreadObject> obj;
	if(m_ThreadPool.find(thread, obj) == 0)
	{
		pthread_cancel(thread);
		m_ThreadPool.remove(thread);
	}
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	CMsgDeque<pthread_t> vThreads;
	m_Threads.Swap(vThreads);
	pthread_t threadTemp;
	while(vThreads.PopFront(threadTemp))
	{
		if(thread != threadTemp)
		{
			m_Threads.PushBack(threadTemp);
		}
	}
}

void CThreadPool::ClearThreadPool()
{
	m_Threads.Clear();
	CTMap<pthread_t, boost::shared_ptr<CThreadObject> > mapThreadPool;
	m_ThreadPool.swap(mapThreadPool);
	vector<boost::shared_ptr<CThreadObject> > vThreadOjbects;
	mapThreadPool.get_all_values(vThreadOjbects);
	for(int i=0; i<vThreadOjbects.size(); ++i)
	{
		vThreadOjbects[i].get()->CancleThreadObject();
	}
}

pthread_t CThreadPool::GetBestThread(SCHEDULING_ALGORITHM type)
{
	pthread_t threadID;
	switch(type)
	{
	case FCFS_ALGORITHM:
		{
			threadID = schedulingFCFS(*this);
			if(threadID == 0)
			{
				threadID = schedulingHRP(*this);
			}
		}
		break;
	case HRP_ALGORITHM:
		{
			threadID = schedulingHRP(*this);
		}
		break;
	default:threadID = -1;
	}

	return threadID;
}

} /* namespace lynetx */
