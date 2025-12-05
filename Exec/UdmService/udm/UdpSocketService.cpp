#include "uv.h"
#include <Winsock2.h>
#include "UdpSocketService.h"
#include "UdmMessage.h"
#include "ByteArray.h"
#include "log.h"
#include "topicList.pb.h"
#include "internal.h"
#include "UDMSerBusiness.h"

using namespace SEF::Udm;
using namespace SEF::COMMON;
using namespace std;

class Subscriber {
public:
	Subscriber()
	{

	}
	~Subscriber()
	{

	}
	void setAddress(const struct sockaddr* addr)
	{
		memcpy(&m_addr, addr, sizeof(struct sockaddr));
	}
	void AddTopic(const string& topic)
	{
		topicSet.insert(topic);
	}
	const struct sockaddr * getAddress()
	{
		return &m_addr;
	}
	bool checkTopicNeed(const string& topic)
	{
		auto it = topicSet.find(topic);
		if (it == topicSet.end())
		{
			return false;
		}
		return true;
	}
	bool operator==(const Subscriber& other) const
	{
		int ret = memcmp(&m_addr, &other.m_addr, sizeof(struct sockaddr));
		if (ret == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	struct sockaddr	m_addr;
	set<string>	topicSet;
};

class UdpServerCtx :public MQNotifier {
public:
	UdpServerCtx()
	{
		m_business = UDMSerBusiness::getInstance();
		m_business->addNotiify(this);
		p_loop = uv_default_loop();
	}

	~UdpServerCtx()
	{
		stop();
	}
	void send(const SEF_Char *data, int len, const struct sockaddr *destAddr)const
	{
		uv_udp_send_t req;
		memset(&req, 0, sizeof(uv_udp_send_t));
		uv_buf_t buffer = uv_buf_init((const_cast<char*>(data)), len);
		int ret = uv_udp_send(&req, p_server, &buffer, 1, destAddr, nullptr);
		if (ret != 0)
		{
			LOGFMTE("udp service %s %d send to %s failed", m_ip.c_str(), m_port, generateKey(destAddr).c_str());
		}
	}
	void recvMsg(ssize_t  nread, const uv_buf_t* buf, const struct sockaddr* addr)
	{
		std::shared_ptr<UdmMessage >jsonMsg = std::make_shared<UdmMessage>();
		UdmMsg msg;
		if (msg.deserialize(buf->base, buf->len) == true)
		{
			if (msg.GetMsgType() == ValueType::VALUE_TYPE_SUBSCRIBE)
			{
				string key = generateKey(addr);
				auto it = subscriberMap.find(key);
				if (it == subscriberMap.end())
				{
					std::shared_ptr<Subscriber> suber = std::make_shared<Subscriber>();
					suber->setAddress(addr);
					suber->AddTopic(msg.GetTopicName());
					subscriberMap.insert(make_pair(key, suber));
				}
				else
				{
					std::shared_ptr<Subscriber> suber = it->second;
					suber->AddTopic(msg.GetTopicName());
				}
			}
			else if (msg.GetMsgType() == ValueType::VALUE_TYPE_QUERY_TOPICNODE)
			{
				UdmMessage jsonMsg(msg);
				int step = 20;
				std::list<UdmMessage> list;
				bool res = m_business->onRequestAllItems(jsonMsg, 5, list);
				if (res == true)
				{
					for(UdmMessage response:list)
					{
						SEF_Size_t size = 0;
						const void* data = response.GetBytes(size);
						send(static_cast<const SEF_Char*>(data),size, addr);
					}
				}
			}
			else
			{
				std::shared_ptr<UdmMessage >udmMessage = std::make_shared<UdmMessage>(msg);
				m_business->onRecv(udmMessage);
			}
		}
	}
	string generateKey(const struct sockaddr* addr)const
	{
		char ip[64] = { 0 };
		const sockaddr_in* in = (const sockaddr_in*)addr;
		uv_ip4_name(in, ip, 64);
		SEF_Uint16 port = 0;
		SwapEndian::swap2local(in->sin_port, port);
		return string(ip) +":" + to_string(port);
	}

	bool setAddress(const string& ip, const int port)
	{
		struct sockaddr_in addr;
		if (p_server != nullptr)
		{
			return false;
		}

		int ret = uv_ip4_addr(ip.c_str(), port, &addr);
		if (ret != 0)
		{
			return false;
		}
		p_server = new uv_udp_t();
		ret = uv_udp_init(p_loop, p_server);
		if (ret != 0)
		{
			return false;
		}
		ret = uv_udp_bind(p_server, (const sockaddr *)&addr, UV_UDP_REUSEADDR);
		if (ret != 0)
		{
			return false;
		}
		uv_udp_recv_start(p_server,
			[](uv_handle_t* handle, size_t size, uv_buf_t* buf)
			{
				UdpServerCtx* ctx = static_cast<UdpServerCtx*>(handle->data);
				buf->base = new char[size];
				buf->len = size;
			},
			[](uv_udp_t* handle, ssize_t  nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned int flages)
			{
				UdpServerCtx* ctx = static_cast<UdpServerCtx*>(handle->data);
				if (nread > 0)
				{
					ctx->recvMsg(nread, buf, addr);
				}
				delete[] buf->base;
			}
		);
		p_server->data = this;
		m_ip = ip;
		m_port = port;
		return true;
	}

	bool start()
	{
		if (m_isRunning == false)
		{
			m_isRunning = true;
			m_thread = std::thread(&UdpServerCtx::loop, this);
			return true;
		}
		return false;
	}
	void stop()
	{
		m_isRunning = false;
		uv_stop(p_loop);
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		if (p_server)
		{
			uv_udp_recv_stop(p_server);
		}
	}

	void loop()
	{
		do {
			uv_run(p_loop, UV_RUN_DEFAULT);
		} while (m_isRunning);
	}

	void onCommonMessage(std::shared_ptr <UdmMessage> message)const override
	{
		SEF_Size_t size = 0;
		UdmMsg msg;
		msg.SetMsgType(message->GetMsgType());
		msg.SetSourceName(message->GetSourceName());
		msg.SetTimestamp(message->GetTimestamp());
		msg.SetTopicName(message->GetTopicName());
		msg.SetVariableName(message->GetVariableName());
		msg.SetVariableUnit(message->GetVariableUnit());
		msg.SetValue(message->GetValue());

		ByteArray array;
		const ByteArray& bytes = *(msg.GetValue());
		ValueType type = msg.GetMsgType();
		msg.serialize(bytes, type, msg.GetTopicName(), msg.GetVariableName(), msg.GetVariableUnit());
		const void *data = msg.GetBytes(size);
		for (const auto &pair: subscriberMap)
		{
			std::shared_ptr<Subscriber> suber = pair.second;
			bool ret = suber->checkTopicNeed(msg.GetTopicName());
			if (ret == true)
			{
				send((const SEF_Char *)data, (int)size, suber->getAddress());
			}
		}
	}


public:
	uv_loop_t*			p_loop = nullptr;
	std::string			m_ip;
	int					m_port;
	uv_udp_t*			p_server = nullptr;
	std::thread			m_thread;
	UDMSerBusiness*		m_business = nullptr;
	bool				m_isRunning = false;
	unordered_map<string, std::shared_ptr<Subscriber>>	subscriberMap;
};


/***************************************************************************************/
SEF::UdmService::UdpSocketService::UdpSocketService()
{
}

SEF::UdmService::UdpSocketService::~UdpSocketService()
{
}

bool SEF::UdmService::UdpSocketService::init(const char* config)
{
	if (!m_ctx)
	{
		m_ctx = new UdpServerCtx();
	}
	return true;
}

bool SEF::UdmService::UdpSocketService::init(unsigned int frequency, unsigned int timeout)
{
	if (!m_ctx)
	{
		m_ctx = new UdpServerCtx();
	}
	return true;
}

bool SEF::UdmService::UdpSocketService::addAddress(const UdmAddress& info)
{
	if ((ProtocolType)info.protocol != ProtocolType::PROTOCOL_TYPE_UDP||(DirectType)info.isLocal != DirectType::LOCAL)
	{
		return false;
	}
	return m_ctx->setAddress(info.ip, info.port);
}

bool SEF::UdmService::UdpSocketService::start()
{
	return m_ctx->start();
}

void SEF::UdmService::UdpSocketService::stop()
{
	m_ctx->stop();
}
