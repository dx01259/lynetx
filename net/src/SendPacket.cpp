/*
 * SendPacket.cpp
 *
 *  Created on: 2015年9月11日
 *      Author: dengxu
 */

#include "SendPacket.h"
#include "InetConnectionPool.h"
#include "TcpConnectionPool.h"
#include "../../base/src/commfunc.h"
#include "../../base/src/logdef.h"

namespace lynetx {

namespace net {

SendPacket::SendPacket():
		m_threadID(0)
{

}

SendPacket::~SendPacket()
{

}

bool SendPacket::Start(const size_t thread_numbers)
{
	check_zero(pthread_attr_init(&m_attr));
	check_zero(pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM));
	check_zero(pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED));
	for(size_t i=0; i<thread_numbers; ++i)
	{
		check_zero(pthread_create(&m_threadID, &m_attr, send_proc, this));
	}
	return true;
}

void SendPacket::Stop()
{
	m_SendPackets.AbortWait();
}

bool SendPacket::PushSendPacket(PacketStruct *packet)
{
	if(packet)
	{
		boost::shared_ptr<PacketStruct> data(packet);
		this->m_SendPackets.PushBack(data);
		return true;
	}
	return false;
}

ssize_t SendPacket::WriteSendPacket(PacketStruct *packet)
{
	if(packet)
	{
		ssize_t sendSize = 0;
		string &host = packet->host;
		switch(packet->type)
		{
		case SEND_TCP_PACKET:
			{
				TcpConnectionPtr tcpPtr;
				CTMap<string, TcpConnectionPtr> *mapTcp = NULL;
				mapTcp = CTcpConnectionPool::Instance()->GetMapTcpConnection();
				if(mapTcp->find(host, tcpPtr)==0)
				{
					sendSize = tcpPtr.get()->Write(*packet->buff.get());
				}
			}
			break;
		case SEND_UDP_PACKET:
			{

			}
			break;
		case SEND_HTTP_PACKET:
			{

			}
			break;
		default:
			TRACE_MSG(LOG_ERRORS, 100, "The type of packet to send is not supported.");
			break;
		}
		return sendSize;
	}
	return -1;
}

void *SendPacket::send_proc(void *arg)
{
	SendPacket *obj = (SendPacket *)arg;
	if(obj)
	{
		while(!obj->GetPacketsDeque()->isAbort())
		{
			boost::shared_ptr<PacketStruct> packet;
			if(obj->GetPacketsDeque()->PopFront(packet))
			{
				ssize_t ret = obj->WriteSendPacket(packet.get());
				CIOBufferPtr *ioPtr = packet.get()->buff.get();
				TRACE_MSG(LOG_DEBUGS, 100+ioPtr->GetLength(),
						"Send Data: %s is %s", ioPtr->ReadPosition(), ret>0 ? "successful":"failed")
			}
		}
	}
	return (void *)0;
}

} /* namespace net */

} /* namespace lynetx */
