/*
 * SocketOps.cpp
 *
 *  Created on: 2015年9月8日
 *      Author: dengxu
 */

#include "SocketOps.h"
#include "../../base/src/logdef.h"
#include <boost/implicit_cast.hpp>
#include <fcntl.h>

namespace lynetx {

namespace net {

namespace SocketOps {

typedef struct sockaddr SA;

const SA *sockaddr_cast(const struct sockaddr_in *addr)
{
	return static_cast<const SA *>(boost::implicit_cast<const void *>(addr));
}

SA *sockaddr_cast(struct sockaddr_in *addr)
{
	return const_cast<SA *>(sockaddr_cast(const_cast<const sockaddr_in *>(addr)));
}

int setAsyncSocket(int sockfd)
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

int getSocketError(int sockfd)
{
	int optval = 0;
	socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
	if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
	{
		return errno;
	}
	return optval;
}

bool getLocalAddr(int sockfd, struct sockaddr_in &localAddr)
{
	bzero(&localAddr, sizeof(localAddr));
	socklen_t addrlen = static_cast<socklen_t>(sizeof(localAddr));
	if(getsockname(sockfd, sockaddr_cast(&localAddr), &addrlen) < 0)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "Get the local address is error");
		return false;
	}
	return true;
}

bool getPeerAddr(int sockfd, struct sockaddr_in &peeraddr)
{
	bzero(&peeraddr, sizeof(peeraddr));
	socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
	if(getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "Get the peer address is error");
		return false;
	}
	return true;
}

ssize_t read(int sockfd, void *buf, size_t count)
{
	return ::read(sockfd, buf, count);
}

ssize_t  write(int sockfd, const void *buf, size_t count)
{
	return ::write(sockfd, buf, count);
}

ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt)
{
	return ::readv(sockfd, iov, iovcnt);
}

ssize_t writev(int sockfd, const struct iovec *iov, int iovcnt)
{
	return ::write(sockfd, iov, iovcnt);
}

bool isSelfConnect(int sockfd)
{
	struct sockaddr_in localaddr, peeraddr;
	getLocalAddr(sockfd, localaddr);
	getPeerAddr(sockfd, peeraddr);
	return localaddr.sin_port == peeraddr.sin_port
			&& localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

} /* namespace SocketOps */

} /* namespace net */

} /* namespace lynetx */
