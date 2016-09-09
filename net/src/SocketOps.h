/*
 * SocketOps.h
 *
 *  Created on: 2015年9月8日
 *      Author: dengxu
 */

#ifndef NET_SRC_SOCKETOPS_H_
#define NET_SRC_SOCKETOPS_H_

#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <bits/socket.h>

namespace lynetx {

namespace net {

namespace SocketOps {

int setAsyncSocket(int sockfd);

int getSocketError(int sockfd);

bool getLocalAddr(int sockfd, struct sockaddr_in &localAddr);

bool getPeerAddr(int sockfd, struct sockaddr_in &peeraddr);

bool isSelfConnect(int sockfd);

ssize_t read(int sockfd, void *buf, size_t count);

ssize_t  write(int sockfd, const void *buf, size_t count);

ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);

ssize_t writev(int sockfd, const struct iovec *iov, int iovcnt);

} /* namespace SocketOps */

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_SOCKETOPS_H_ */
