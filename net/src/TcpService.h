/*
 * TcpService.h
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#ifndef NET_SRC_TCPSERVICE_H_
#define NET_SRC_TCPSERVICE_H_

#include"AsyncSocket.h"
#include"TcpConnectionPool.h"
#include"../../base/src/EpollWatch.h"
#include"../../base/src/IOBufferPtr.h"
#include"../../base/src/ThreadPool.h"
#include"../../base/src/Mutex.h"
#include"../../base/src/TMap.h"
#include"../../base/src/logdef.h"
#include"../../base/src/MsgDeque.h"
#include<boost/shared_ptr.hpp>
#include<pthread.h>
#include<semaphore.h>

namespace lynetx {

namespace net {

class CTcpService;

typedef void (*ConnectionFunc)(const void *arg, const string &host, const TcpConnectionPtr &conn);
typedef ssize_t (*MessageFunc)(const boost::shared_ptr<CIOBufferPtr> &buffptr, const TcpConnectionPtr &conn);
typedef void (*DisconnectionFunc)(CTcpService *pTcpService, const string &host);
typedef void (*WakeupEventFunc)(void *arg);

void _Connection(const void *arg, const string &host, const TcpConnectionPtr &conn);
ssize_t _MessageRecv(const boost::shared_ptr<CIOBufferPtr> &buffptr, const TcpConnectionPtr &conn);
void _Disconnection(CTcpService *pTcpService, const string &host);

class CTcpService :
		public CAsyncSocket,
		public CEpollWatch
{
public:
	CTcpService();
	virtual ~CTcpService();

	typedef struct RECV_OBJECT
	{
		RECV_OBJECT():
			_service(NULL)
		{}
		CTcpService *_service;
		epoll_event	_eve;
	}RecvObject;
public:
	int Start(const short port, const size_t sThreadPoolSize=10, int sigset=SIGUSR1);
	void Stop();
	void AddTcpConnection(const string &host, TcpConnectionPtr conn)
	{
		int ret = CTcpConnectionPool::Instance()->GetMapTcpConnection()->set(host, conn);
		TRACE_MSG(LOG_DEBUGS, 200, "Add new client TCP connection of %s is %s.", host.c_str(), ret==0?"success":"failed");
	}
	void RemovTcpConnection(const string &host)
	{
		CTcpConnectionPool::Instance()->GetMapTcpConnection()->remove(host);
		TRACE_MSG(LOG_DEBUGS, 100, "Remove client TCP connection of %s.", host.c_str());
	}
	int FindTcpConnection(const string &host, TcpConnectionPtr &conn)
	{
		return CTcpConnectionPool::Instance()->GetMapTcpConnection()->find(host, conn);
	}
	void ClearTcpConnection()
	{
		CTcpConnectionPool::Instance()->GetMapTcpConnection()->clear();
		TRACE_MSG(LOG_DEBUGS, 100, "Clear the Client TCP conncetions");
	}
public:
	void SetConnectionFunc(ConnectionFunc cfunc){defaultConnectionFunc=cfunc;}
	void SetMessageFunc(MessageFunc mfunc){defaultMessageFunc=mfunc;}
	void SetDisConnectionFunc(DisconnectionFunc dfunc){defaultDisconnectionFunc=dfunc;}
	void GetTimeoutSocket(vector<int> &vSockets);
public:
	int IoEpollWait(int timeout);
protected:
	static void *thread_proc(void *arg);
	static void  clean_func(void *arg);
	static void *recv_message(void *arg);
protected:
	ConnectionFunc		defaultConnectionFunc;
	MessageFunc			defaultMessageFunc;
	DisconnectionFunc 	defaultDisconnectionFunc;
	WakeupEventFunc		defaultWakeupEventFunc;
private:
	bool m_ServiceStarted;				//判断服务是否启动
	int  m_timeout;						//epoll_pwait的超时设置
	sem_t m_semt;						//线程异步信号量
	CMutex	m_mutex;						//进程内部线程同步句柄
	pthread_t m_threadID;				//TCP服务的内部线程句柄
	pthread_attr_t m_attr;				//线程的属性
	CThreadPool m_RecvThreadPool;	//接收线程池，用来处理接收客户的端数据
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_TCPSERVICE_H_ */
