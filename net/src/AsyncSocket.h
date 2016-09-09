/*
 * AsyncSocket.h
 *
 *  Created on: 2015年9月4日
 *      Author: dengxu
 */

#ifndef NET_SRC_ASYNCSOCKET_H_
#define NET_SRC_ASYNCSOCKET_H_

#include"../../base/src/Socket.h"

namespace lynetx {

namespace net {

class CAsyncSocket :
		public CSocket
{
public:
	CAsyncSocket();
	virtual ~CAsyncSocket();
	inline int GetSocket()
	{
		return m_socket;
	}
public:
	int CreateSocket(const char *ip="127.0.0.1", const short port=10000, int protocol=PROTO_TCP);
	void CloseSocket();
	virtual ssize_t RecvData(void *buf, size_t len);
	virtual ssize_t SendData(const char *buf, size_t len);
protected:
	static int SetAsyncSocket(int sockfd);
private:
	int m_socket;			//保存SOCKET的描述符
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_ASYNCSOCKET_H_ */
