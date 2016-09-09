/*
 * SendPacket.h
 *
 *  Created on: 2015年9月11日
 *      Author: dengxu
 */

#ifndef NET_SRC_SENDPACKET_H_
#define NET_SRC_SENDPACKET_H_

#include<pthread.h>
#include<boost/shared_ptr.hpp>
#include"../../base/src/datatype.h"
#include"../../base/src/IOBufferPtr.h"
#include"../../base/src/MsgDeque.h"


namespace lynetx {

namespace net {

enum SEND_PACKET_TYPE
{
	SEND_TCP_PACKET = 0,
	SEND_UDP_PACKET = 1,
	SEND_HTTP_PACKET = 2,
};

typedef struct PACKET_STRUCT
{
	u_short 		type;			//数据包的类型
	std::string	host;			//主机地址（IP and PORT，eg: 127.0.0.1:8000）
	boost::shared_ptr<CIOBufferPtr> buff;	//需要发送的数据
}PacketStruct;

class SendPacket
{
public:
	SendPacket();
	virtual ~SendPacket();
public:
	bool Start(const size_t thread_numbers);
	void Stop();
	bool PushSendPacket(PacketStruct *packet);
	ssize_t WriteSendPacket(PacketStruct *packet);
protected:
	CMsgDeque<boost::shared_ptr<PacketStruct> > *GetPacketsDeque()
	{
		return &m_SendPackets;
	}
	static void *send_proc(void *arg);
private:
	pthread_t m_threadID;				//TCP服务的内部线程句柄
	pthread_attr_t m_attr;				//线程的属性
	CMsgDeque<boost::shared_ptr<PacketStruct> > m_SendPackets;
};

} /* namespace net */

} /* namespace lynetx */

#endif /* NET_SRC_SENDPACKET_H_ */
