/*
 * ThreadObject.cpp
 *
 *  Created on: 2015年9月1日
 *      Author: dengxu
 */

#include"ThreadObject.h"
#include"commfunc.h"
#include"logdef.h"
#include"Timeout.h"
#include<boost/make_shared.hpp>

namespace lynetx {

const int WAIT_QUIT_THREAD_TIME=30000;//30微妙

CThreadObject::CThreadObject()
{
	memset(&m_thread, 0, sizeof(thread_struct));
	InitObject();
}

CThreadObject::~CThreadObject()
{
	pthread_attr_destroy(&m_attr);
	sem_destroy(&m_ReadyRunning);
}

void CThreadObject::InitObject()
{
	check_zero(sem_init(&m_ReadyRunning, 0, 0));
	check_zero(pthread_attr_init(&m_attr));
	check_zero(pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM));
	check_zero(pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED));
}

bool CThreadObject::CreateThreadObject()
{
	check_zero(pthread_create(&m_thread.thread, &m_attr, thread_proc, this));
	return true;
}

void CThreadObject::ThreadObjectWaitquit()
{
	struct timespec spec;
	CTimeout::timeofNowTime(spec, WAIT_QUIT_THREAD_TIME);
	sem_timedwait(&m_ReadyRunning, &spec);
}

void CThreadObject::CancleThreadObject(const size_t waitusecs)
{
	size_t busying;
	thread_status status;
	GetThreadStatus(busying, status);
	m_pTasks.AbortWait();
	ThreadObjectWaitquit();
	TRACE_MSG(LOG_DEBUGS, 100, "发送取消线程:0x%x指令", m_thread.thread);
}

bool CThreadObject::AddThreadTask(const pthread_func func, const void *args)
{
	if(func){
		TaskPtr ptask = TaskPtr(boost::make_shared<thread_task>());
		assert(ptask);
		ptask.get()->function = func;
		ptask.get()->parameter= const_cast<void *>(args);
		if(m_pTasks.PushBack(ptask))
		{
			SetThreadStatus(++m_thread.busying, IS_WAITING);
		}
	}
	else{
		TRACE_MSG(LOG_ERRORS, 100, "Add Thread task is error that function pointer is null");
	}
	return false;
}

void *CThreadObject::thread_proc(void *arg)
{
	CThreadObject *objp = (CThreadObject *)arg;
	cleanup_push_thread(clean_func, objp);
	if(objp)
	{
		TRACE_MSG(LOG_DEBUGS, 100, "线程:0x%x开始获取任务！", pthread_self());
		TaskPtr pTask;
		objp->SetThreadStatus(objp->m_thread.busying, IS_SLEEP);
		while(!objp->m_pTasks.isAbort())
		{
			try{
				if(objp->m_pTasks.PopFront(pTask, -1))
				{
					objp->SetThreadStatus(--objp->m_thread.busying, IS_RUNNING);
					thread_task *task = pTask.get();
					assert(task);
					task->function(task->parameter);//运行线程之中添加的任务
				}
				else{
					continue;
				}
			}
			catch(std::bad_alloc &e)
			{
				objp->SetThreadStatus(objp->m_thread.busying, IS_SLEEP);
				TRACE_MSG(LOG_ERRORS, 100, "分配内存发生异常: %s", e.what());
				continue;
			}
			catch(...)
			{
				objp->SetThreadStatus(objp->m_thread.busying, IS_SLEEP);
				TRACE_MSG(LOG_ERRORS, 100, "线程处理任务的时候发生未知异常！");
				continue;
			}
			objp->SetThreadStatus(objp->m_thread.busying, IS_SLEEP);
		}
		TRACE_MSG(LOG_DEBUGS, 100, "线程:0x%x处理完所有任务！", pthread_self());
	}
	cleanup_pop_thread(1);
	return (void *)0;
}

void CThreadObject::clean_func(void *arg)
{
	CThreadObject *objp = (CThreadObject *)arg;
	if(objp)
	{
		objp->m_pTasks.Clear();
		objp->SetThreadStatus(objp->m_thread.busying, IS_EXITED);
		objp->ReadyRunningPost();
	}
	TRACE_MSG(LOG_DEBUGS, 100, "线程:0x%x退出！", pthread_self());
}

} /* namespace lynetx */
