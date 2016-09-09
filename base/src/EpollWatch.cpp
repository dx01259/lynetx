/*
 * EpollWatch.cpp
 *
 *  Created on: 2015年9月1日
 *      Author: dengxu
 */

#include"EpollWatch.h"
#include"logdef.h"
#include"Mutex.h"
#include<string.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>

namespace lynetx {


CEpollWatch::CEpollWatch():
		m_bEpollWait(false),
		m_wakeupfd(0),
		m_enableWakeup(false)
{
	memset(&m_object, 0, sizeof(EpollWaitOjbect));
	m_object.eventSize = 1000;
	m_object.epollfd   = 0;
	m_object.vEvents.resize(m_object.eventSize);
}

CEpollWatch::~CEpollWatch()
{
	if( 0 != m_object.epollfd)
	{
		close(m_object.epollfd);
		m_object.epollfd = 0;
	}
	close(this->m_wakeupfd);
}

int CEpollWatch::EpollCreate()
{
	if(m_object.epollfd == 0 )
	{
		m_object.epollfd = epoll_create1(EPOLL_CLOEXEC);
		if(m_object.epollfd < 0)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Call epoll_create1 to create epoll object is error");
			return -1;
		}
	}

	return m_object.epollfd;
}

int CEpollWatch::EpollCtl(const int eopt, const int fd, const int events)
{
	int ret = -1;
	if(m_object.epollfd > 0)
	{
		AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

		m_object.event.events	= events;
		m_object.event.data.fd	= fd;
		ret = epoll_ctl(m_object.epollfd, eopt, fd, &m_object.event);
		if(-1 == ret)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Call epoll_ctl to enroll epoll object is error");
			return -2;
		}
		if(eopt == EPOLL_CTL_ADD) {
			++m_object.maxNumbers;
			TRACE_MSG(LOG_DEBUGS, 100, "监听IO:%d 添加套接字:%d 成功，总共监听数: %d！",
					m_object.epollfd ,fd, m_object.maxNumbers);
		}
		else if(EPOLL_CTL_DEL==eopt){
			--m_object.maxNumbers;
			TRACE_MSG(LOG_DEBUGS, 100, "监听IO:%d 删除套接字:%d 成功，总共监听数: %d！",
					m_object.epollfd, fd, m_object.maxNumbers);
		}
	}

	return ret;
}

int CEpollWatch::EpollAdd(const int fd, const int events)
{
	return EpollCtl(EPOLL_CTL_ADD, fd, events);
}

int CEpollWatch::EpollModify(const int fd, const int events)
{
	return EpollCtl(EPOLL_CTL_MOD, fd, events);
}

int CEpollWatch::EpollDel(const int fd, const int events)
{
	return EpollCtl(EPOLL_CTL_DEL, fd, events);
}

int CEpollWatch::IoEpollWait(int timeout)
{
	return 0;
}

int CEpollWatch::EnableWakeup()
{
	m_wakeupfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	assert(m_wakeupfd > 0);
	m_enableWakeup = true;
	return m_wakeupfd;
}

void CEpollWatch::DisableWakeup()
{
	if(m_enableWakeup)
	{
		if(EpollDel(m_wakeupfd, EPOLLET | EPOLLIN) ==0 )
		{
			m_enableWakeup = false;
			close(m_wakeupfd);
			m_wakeupfd = 0;
		}
	}
}

void CEpollWatch::PushWakeup2Epoll(const int events)
{
	if(!m_enableWakeup)
	{
		EnableWakeup();
	}
	EpollAdd(m_wakeupfd, events);
}

void CEpollWatch::EpollWakeup()
{
	uint64_t one = 1;
	ssize_t n = write(m_wakeupfd, &one, sizeof(one));
	if(n != sizeof(one))
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "EpollWakeup happy error");
	}
}

void CEpollWatch::EpollHandleRead()
{
	uint64_t one = 1;
	ssize_t n = read(m_wakeupfd, &one, sizeof(one));
	if(n != sizeof(one))
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "EpollHandleRead happy error");
	}
}

} /* namespace lynetx */


