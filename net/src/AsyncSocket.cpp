/*
 * AsyncSocket.cpp
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#include"AsyncSocket.h"
#include"../../base/src/logdef.h"
#include<fcntl.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

namespace lynetx {

namespace net {

CAsyncSocket::CAsyncSocket() :
		m_socket(-1)
{

}

CAsyncSocket::~CAsyncSocket()
{
}

int CAsyncSocket::CreateSocket(const char *ip, const short port, int protocol)
{
	int type = (protocol!=PROTO_UDP ? SOCK_STREAM:SOCK_DGRAM);
	int sockfd = Socket(AF_INET, type, 0);
	if(sockfd > 0)
	{
		m_socket = sockfd;
		struct sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(port);
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int ret = Bind(m_socket, (struct sockaddr *)&servaddr, sizeof(servaddr));
		SetAsyncSocket(m_socket);
		if(ret == 0 && type == SOCK_STREAM)
		{
			ret = Listen(m_socket, SOMAXCONN);
			if(0 != ret)
			{
				TRACE_ERR(LOG_ERRORS, errno, 100, "Listen the socket object is error");
				return ret;
			}
		}
		return sockfd;
	}
	return 0;
}

void CAsyncSocket::CloseSocket()
{
	if(-1 != this->m_socket)
	{
		close(this->m_socket);
		this->m_socket = -1;
	}
}

ssize_t CAsyncSocket::RecvData(void *buf, size_t len)
{
	return Recv(this->m_socket, buf, len, MSG_DONTWAIT|MSG_CMSG_CLOEXEC);
}

ssize_t CAsyncSocket::SendData(const char *buf, size_t len)
{
	return Send(this->m_socket, buf, len, MSG_DONTWAIT|MSG_CMSG_CLOEXEC);
}

int CAsyncSocket::SetAsyncSocket(int sockfd)
{
	if(sockfd > 0)
	{
		int flags = fcntl(sockfd, F_GETFL, 0);
		if(-1 == flags)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "fcntl change the socket object is error");
			return -1;
		}
		flags |= SOCK_NONBLOCK;
		flags = fcntl(sockfd, F_SETFL, flags);
		if(-1 == flags)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "fcntl change the socket object is error");
			return -1;
		}
		TRACE_MSG(LOG_DEBUGS, 100, "Socket %d set SOCK_NONBLOCK is success", sockfd);
		return sockfd;
	}
	return -2;
}

} /* namespace net */

} /* namespace lynetx */
