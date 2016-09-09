/*
 * TcpService.cpp
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#include "TcpService.h"
#include "SocketOps.h"
#include"../../base/src/CharEncode.h"
#include <boost/make_shared.hpp>

namespace lynetx {

namespace net {

void _Connection(const void *arg, const string &host, const TcpConnectionPtr &conn)
{
	assert(conn);
	CTcpService *_service = (CTcpService *)arg;
	if(_service)
	{
		_service->AddTcpConnection(host, conn);
	}
}

ssize_t _MessageRecv(const boost::shared_ptr<CIOBufferPtr> &buffptr, const TcpConnectionPtr &conn)
{
	assert(buffptr);
	boost::shared_ptr<CIOBufferPtr> ptr = 	buffptr;
	CIOBufferPtr utf8Buff(ptr.get()->GetLength()*2);
	size_t writeBytes = utf8Buff.WriteableBytes(), buffLen=ptr.get()->GetLength();
	char *inbuf = reinterpret_cast<char *>(ptr.get()->ReadPosition());
	char *outbuf= reinterpret_cast<char *>(utf8Buff.WritePosition());
	CharEncode::Instance()->Convert(&inbuf, &buffLen, &outbuf, &writeBytes);
	TRACE_MSG(LOG_MESSAGES, 100+utf8Buff.GetLength(), "Recv: %s", utf8Buff.ReadPosition());
	return 0;
}

void _Disconnection(CTcpService *pTcpService, const string &host)
{
	assert(pTcpService);
	if(pTcpService)
	{
		pTcpService->RemovTcpConnection(host);
	}
}

void _WakeupEvent(void *arg)
{
	assert(arg);
	CTcpService *_service = (CTcpService *)arg;
	_service->DisableEpollWait();
}

CTcpService::CTcpService() :
		defaultConnectionFunc(_Connection),
		defaultMessageFunc(_MessageRecv),
		defaultDisconnectionFunc(_Disconnection),
		defaultWakeupEventFunc(_WakeupEvent),
		m_ServiceStarted(false),
		m_threadID(0),
		m_timeout(-1)
{
	m_bEpollWait = true;//不同命名空间，只能通过赋值，不能初始化。
}

CTcpService::~CTcpService()
{
	if(this->m_bEpollWait)
	{
		this->Stop();
	}
	pthread_attr_destroy(&m_attr);
	sem_destroy(&m_semt);
}

int CTcpService::Start(const short port, const size_t sThreadPoolSize, int sigset)
{
	check_zero(sem_init(&m_semt, 0, 0));
	check_zero(pthread_attr_init(&m_attr));
	check_zero(pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM));
	check_zero(pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED));
	this->m_RecvThreadPool.CreateThreadPool(sThreadPoolSize);

	int sockfd = CreateSocket("127.0.0.1", port);
	if(sockfd < 0)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "Create Socket object is failed.");
		return -1;
	}
	CharEncode::Instance()->InitEncode();
	SetEpollSignalObject(sigset);
	check_zero(pthread_create(&m_threadID, &m_attr, thread_proc, this));

	m_ServiceStarted = true;
	return 0;
}

void CTcpService::Stop()
{
	if(m_ServiceStarted)
	{
		CharEncode::Instance()->DestoryEncode();
		m_ServiceStarted = false;
		m_timeout 	= 0;
		m_bEpollWait = false;
		m_enableWakeup ? EpollWakeup(),0:pthread_cancel(m_threadID);
	}
}

void CTcpService::GetTimeoutSocket(vector<int> &vSockets)
{
	vector<TcpConnectionPtr> vfd2Timeout;
	CTcpConnectionPool::Instance()->GetMapTcpConnection()->get_all_values(vfd2Timeout);
	if(vfd2Timeout.size() > 0)
	{
		vSockets.clear();
		vector<TcpConnectionPtr>::iterator iter=vfd2Timeout.begin();
		for(; iter != vfd2Timeout.end(); ++iter)
		{
			if((*iter).get()->GetTimeoutObject().IsTimeout())
			{
				vSockets.push_back((*iter).get()->GetClientfd());
			}
		}
	}
}

int CTcpService::IoEpollWait(int timeout)
{
	TRACE_MSG(LOG_DEBUGS, 100, "epoll listen socket begin !");
	m_timeout = timeout;
	while(m_bEpollWait)
	{
		SafeChangeEvents();//此函数必须添加，否则会发生段错误。
		printf("m_object.epollfd=%d, m_object.maxNumbers=%d\n",
				m_object.epollfd, m_object.maxNumbers);
		int ret = epoll_pwait(m_object.epollfd, &m_object.vEvents[0], m_object.maxNumbers, m_timeout, &m_object.siget);
		printf("epoll_pwait return\n");
		if(0 ==ret ) continue;//timeout
		else if(0 > ret)
		{
			if(EINTR==errno || EAGAIN==errno)
				continue;
			TRACE_ERR(LOG_ERRORS, errno, 100, "epoll_pwait happy error");
			return -1;
		}
		for(int i=0; i<ret && i<m_object.maxNumbers; ++i)
		{
			printf("m_object.maxNumbers=%d, ret=%d, i=%d.\n", m_object.maxNumbers, ret, i);
			int clientfd = 0;
			struct sockaddr_in addr;
			socklen_t addrlen = sizeof(struct sockaddr);
			epoll_event &event = m_object.vEvents[i];
			if(GetSocket() == event.data.fd)
			{
				while(0<(clientfd = Accept(event.data.fd, (struct sockaddr *)&addr, &addrlen))){
					TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d请求连接!", clientfd);
					SetAsyncSocket(clientfd);
					EpollAdd(clientfd, EPOLLET | EPOLLIN);
					TcpConnectionPtr conn = TcpConnectionPtr(new CTcpConnection);
					conn.get()->SetSocket(clientfd, addr);
					assert(defaultConnectionFunc);
					string szIpAndPort = conn.get()->GetInetAddrObject().toIpAndPort();
					defaultConnectionFunc(this, szIpAndPort, conn);
					CTcpConnectionPool::Instance()->GetSocket2InetAddr()->set(clientfd, szIpAndPort);
				}
				if(-1==clientfd && errno!=EAGAIN &&
						ECONNABORTED!=errno && EPROTO!=errno && EINTR!=errno)
				{
					TRACE_ERR(LOG_ERRORS, errno, 100, "Accept socket is error");
				}
				continue;
			}
			else if(m_wakeupfd == event.data.fd && m_enableWakeup)
			{
				//程序被唤醒，用来主动退出处理对应业务
				defaultWakeupEventFunc(this);
			}
			else if(EPOLLIN & event.events)//read data
			{
				pthread_t thread = m_RecvThreadPool.GetBestThread(FCFS_ALGORITHM);
				RecvObject *recvObject = new RecvObject();
				assert(recvObject);
				recvObject->_service = this;
				recvObject->_eve  = event;
				m_RecvThreadPool.AddTask(thread, recv_message, recvObject);
			}
			else if(EPOLLOUT & event.events)//write data
			{
				int fd = event.data.fd;
				string szSendData = (char *)event.data.ptr;
				int datalen = szSendData.size(), sendlen=0;
				while(0 < datalen)
				{
					sendlen=Write(fd, szSendData.c_str(), datalen);
					if(sendlen < 0)
					{
						if(-1 == sendlen && errno != EAGAIN)
						{
							TRACE_ERR(LOG_ERRORS, errno, 100, "Send %s is error", szSendData.c_str());
							continue;
						}
						break;
					}
					datalen -= sendlen;
				}
				TRACE_MSG(LOG_DEBUGS, 20+szSendData.size(), "Send %s is success", szSendData.c_str());
			}
		}
	}
	TRACE_MSG(LOG_DEBUGS, 100, "epoll listen socket end !");

	return 0;
}

void *CTcpService::thread_proc(void *arg)
{
	CTcpService *_service = (CTcpService *)arg;
	cleanup_push_thread(clean_func, _service);
	if(_service)
	{
		_service->EpollCreate();
		_service->PushWakeup2Epoll(EPOLLET | EPOLLIN);
		_service->EpollAdd(_service->GetSocket(), EPOLLET | EPOLLIN );
		_service->IoEpollWait(-1);
	}
	cleanup_pop_thread(1);
	return (void *)0;
}

void  CTcpService::clean_func(void *arg)
{
	CTcpService *_service = (CTcpService *)arg;
	if(_service)
	{
		_service->m_RecvThreadPool.ClearThreadPool();
		vector<TcpConnectionPtr> vTcpConn;
		CTcpConnectionPool::Instance()->GetMapTcpConnection()->get_all_values(vTcpConn);
		vector<TcpConnectionPtr>::iterator iter = vTcpConn.begin();
		for(; iter != vTcpConn.end(); ++iter)
		{
			TcpConnectionPtr &conn = *iter;
			close(conn.get()->GetClientfd());//服务器主动断开客户的端连接的
		}
		_service->ClearTcpConnection();
		_service->CloseSocket();
		sem_post(&_service->m_semt);
	}
}

void *CTcpService::recv_message(void *arg)
{
	if(NULL == arg)
	{
		TRACE_MSG(LOG_ERRORS, 100, "The parameter is null");
		return (void *)-1;
	}
	boost::shared_ptr<RecvObject> obj((RecvObject *)arg);
	epoll_event &eve = obj.get()->_eve;
	CTcpService *pTcpService = obj.get()->_service;
	int fd = eve.data.fd;
	string szPeerAddr;
	struct sockaddr_in addr;
	if(lynetx::net::SocketOps::getPeerAddr(fd, addr))
	{
		CInetAddr InetAddr(addr);
		szPeerAddr = InetAddr.toIpAndPort();
	}
	else
	{
		int errnoInfo = errno;
		if(CTcpConnectionPool::Instance()->GetSocket2InetAddr()->find(fd, szPeerAddr)!=0)
		{
			TRACE_MSG(LOG_ERRORS, 100, "获取套接子: %d地址发生异常！", fd);
			return (void *)-1;
		}
	}
	ssize_t reLen = 0, offset=1024*1024;//1MB
	ssize_t allReadLen = 0;
	boost::shared_ptr<CIOBufferPtr> buffPtr = boost::make_shared<CIOBufferPtr>(offset);
	while((reLen=pTcpService->Read(fd, (void *)buffPtr.get()->WritePosition(), offset)) > 0 )
	{
		buffPtr.get()->skip(reLen);//读了多少字节，就移动位置
		allReadLen += reLen;
		if(buffPtr.get()->WriteableBytes() < offset && reLen == offset)
		{
			TRACE_MSG(LOG_DEBUGS, 100, "BuffPtr size is %d < shoud write size %d", buffPtr.get()->WriteableBytes(), offset);
			buffPtr.get()->Resize(buffPtr.get()->GetLength()+offset);
		}
	}

	if((allReadLen < 0 && errno==ECONNRESET)
			|| (0 == allReadLen) )
	{
		TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d主动关闭连接!", fd);
		pTcpService->EpollDel(fd,  EPOLLET | EPOLLIN);
		close(fd);
		CTcpConnectionPool::Instance()->GetSocket2InetAddr()->remove(fd);
		pTcpService->defaultDisconnectionFunc(pTcpService, szPeerAddr);
		return (void *)-1;
	}
	else if(reLen <0 && reLen != -2)
	{
		TRACE_MSG(LOG_DEBUGS, 100, "接收套接字: %d数据失败，关闭对应连接。", fd);
		pTcpService->EpollDel(fd,  EPOLLET | EPOLLIN);
		close(fd);
		CTcpConnectionPool::Instance()->GetSocket2InetAddr()->remove(fd);
		pTcpService->defaultDisconnectionFunc(pTcpService, szPeerAddr);
		return (void *)-2;
	}
	TcpConnectionPtr conn;
	pTcpService->FindTcpConnection(szPeerAddr, conn);
	_MessageRecv(buffPtr, conn);

	return (void *)0;
}

} /* namespace net */

} /* namespace lynetx */
