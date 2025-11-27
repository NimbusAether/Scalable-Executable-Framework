#include "uv.h"
#include <unordered_map>
#include <map>
#include <Winsock2.h>
#include "UdpSocketUdm.h"
#include "UdmJsonRefMsg.h"
#include "SafeQueue.h"
#include "log.h"
#include "topicList.pb.h"
#include "internal.h"

using SEF::Udm::UdpSocketUdm;

using SEF::Udm::Udm;
using SEF::Udm::RequestCb;
using namespace SEF::COMMON;
using namespace SEF::Udm;
using namespace SEF::Config;
using namespace SEF::Log;
using namespace std;
using namespace UDM;
using namespace SEF::INTERNAL;
static SEF_Uint64 getCurrentSec()
{
	return time(nullptr);
}



class UdpUdmContent {
public:
	UdpUdmContent(list<UdmAddress> &config)
	{ 
		struct sockaddr_in addr;
		m_udpHandle = new uv_udp_t();
		p_loop = uv_default_loop();
		int ret = uv_udp_init(p_loop, m_udpHandle);
		if (ret != 0)
		{
			return;
		}
		for (UdmAddress addr:config)
		{
			if((ProtocolType)addr.protocol != ProtocolType::PROTOCOL_TYPE_UDP)
			{
				continue;
			}
			if ((DirectType)addr.isLocal == DirectType::LOCAL)
			{
				m_localIp = addr.ip;
				m_localPort = addr.port;
			}
			else if ((DirectType)addr.isLocal == DirectType::REMOTE)
			{
				m_dstIp = addr.ip;
				m_dstPort = addr.port;
				int ret = uv_ip4_addr(m_dstIp.c_str(), m_dstPort, &m_dstAddr);
				if (ret != 0)
				{
					return;
				}
			}
		}
		if (m_dstIp.empty() || m_localIp.empty())
		{
			return;
		}

		ret = uv_ip4_addr(m_localIp.c_str(), m_localPort, &addr);
		if (ret != 0)
		{
			return;
		}
		ret = uv_udp_bind(m_udpHandle, (const sockaddr*)&addr, UV_UDP_REUSEADDR);
		if (ret != 0)
		{
			return;
		}

		uv_udp_recv_start(m_udpHandle,
			[](uv_handle_t* handle, size_t size, uv_buf_t* buf)
			{
				UdpUdmContent* ctx = static_cast<UdpUdmContent*>(handle->data);
				buf->base = new char[size];
				buf->len = size;
			},
			[](uv_udp_t* handle, ssize_t  nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned int flages)
			{
				UdpUdmContent* ctx = static_cast<UdpUdmContent*>(handle->data);
				if (nread > 0)
				{
					ctx->recvMsg(nread, buf, addr);
				}
				delete[] buf->base;
			}
		);
		m_udpHandle->data = this;


		m_init = true;
	}
	~UdpUdmContent()
	{
		if (m_init == true)
		{
			uv_udp_recv_stop(m_udpHandle);
		}
	}
	void recvMsg(ssize_t  nread, const uv_buf_t* buf, const struct sockaddr* addr)
	{
		std::shared_ptr<UdmMessage >udmMessage = std::make_shared<UdmMessage>();
		if (udmMessage->deserialize(buf->base, buf->len) == true)
		{
			m_msgQueue.push(udmMessage);
		}
	}

	bool publish(const void *buf, size_t message_size)
	{
		uv_udp_send_t req;
		memset(&req, 0, sizeof(uv_udp_send_t));
		uv_buf_t buffer = uv_buf_init(static_cast<char *>(const_cast<void*>(buf)), message_size);
		int ret = uv_udp_send(&req, m_udpHandle, &buffer, 1, (struct sockaddr*)(&m_dstAddr), nullptr);
		if (ret != 0)
		{
			LOGFMTE("udp service %s %d send to %s %d failed", m_localIp.c_str(), m_localPort, m_dstIp.c_str(), m_dstPort);
		}
		return true;
	}

public:
	uv_loop_t* p_loop = nullptr;
	uv_udp_t* m_udpHandle;
	SEF_Uint64 publishCount = 0;
	bool m_init = false;
	bool m_isRunning = false;
	SafeQueue<std::shared_ptr<UdmMessage>> m_msgQueue;

	string		m_localIp;
	SEF_Ushort m_localPort;

	string		m_dstIp;
	SEF_Ushort m_dstPort;
	sockaddr_in m_dstAddr;
};

UdpSocketUdm::UdpSocketUdm()
{
	udm_type = UdmType::SocketAPI;
}

UdpSocketUdm::~UdpSocketUdm()
{
	m_ctx->m_isRunning = false;
	m_ctx->m_msgQueue.terminate();
	if (m_thread.joinable())
	{
		m_thread.join();
	}
	delete m_ctx;
	m_ctx = nullptr;
}

bool UdpSocketUdm::init()
{
	if (UdpUdmContentInit() == false)
	{
		LOGFMTE("udp udm init failed");
		return false;
	}
	m_thread = std::thread(&UdpSocketUdm::threadCycle, this);
	return true;
}

bool UdpSocketUdm::publish(std::shared_ptr<UdmMessage> message)
{
	UdmMsg udmMsg;
	udmMsg.SetMsgType(message->GetMsgType());
	udmMsg.SetSourceName(message->GetSourceName());
	udmMsg.SetTimestamp(message->GetTimestamp());
	udmMsg.SetTopicName(message->GetTopicName());
	udmMsg.SetVariableName(message->GetVariableName());
	udmMsg.SetVariableUnit(message->GetVariableUnit());
	udmMsg.SetValue(message->GetValue());

	udmMsg.serialize();
	SEF_Size_t size = 0;
	const void* data = udmMsg.GetBytes(size);
	return m_ctx->publish(data, size);
}

bool UdpSocketUdm::_sendRequest(UdmMessage& message, UdmMessage& reply)
{
	UdmMsg udmMsg(message);
	udmMsg.serialize();
	SEF_Size_t size = 0;
	const void* data = udmMsg.GetBytes(size);
	return m_ctx->publish(data, size);
}

bool UdpSocketUdm::subscribe(const string& topic)
{
	UdmMsg udmMsg;
	udmMsg.SetTopicName(topic);
	udmMsg.SetMsgType(ValueType::VALUE_TYPE_SUBSCRIBE);

	udmMsg.serialize();
	SEF_Size_t size = 0;
	const void* data = udmMsg.GetBytes(size);
	return m_ctx->publish(data, size);
}

bool SEF::Udm::UdpSocketUdm::unsubscribe(const string& topic)
{
	return false;
}


bool UdpSocketUdm::pushSubItem(UdmMessage& udmMessage)
{
	SEF_Size_t size = 0;
	const void* data = udmMessage.GetBytes(size);
	return m_ctx->publish(data, size);
}

bool SEF::Udm::UdpSocketUdm::isUdmConnected()
{
	return false;
}


bool UdpSocketUdm::UdpUdmContentInit()
{
	if (m_ctx == nullptr) 
	{
		m_ctx = new UdpUdmContent(udm_address);
	}
	return m_ctx->m_init;
}

void UdpSocketUdm::threadCycle()
{
	while (m_ctx->m_isRunning == true)
	{
		std::shared_ptr<UdmMessage >udmMessage;
		while (m_ctx->m_msgQueue.pop(udmMessage))
		{
			onRecv(udmMessage);
		}
	}
}



