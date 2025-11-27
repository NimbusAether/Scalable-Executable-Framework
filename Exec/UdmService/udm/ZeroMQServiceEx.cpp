#include "ZeroMQServiceEx.h"
#include "UdmMessage.h"
#include "zmq.h"
#include "ByteArray.h"
#include "log.h"
#include "topicList.pb.h"
#include "internal.h"
#include "UDMSerBusiness.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using namespace SEF::UdmService;
using namespace SEF::Udm;
using namespace std;
using  SEF::COMMON::SafeQueue;
using namespace UDM;
using namespace SEF::INTERNAL;
constexpr int sendFlags = ZMQ_DONTWAIT;



static int test_assert_success_message_errno_helper(int rc_,
	const char* msg_,
	const char* expr_,
	int line_)
{
	if (rc_ == -1) {
		char buffer[512] = {0};
		buffer[sizeof(buffer) - 1] =
			0; // to ensure defined behavior with VC++ <= 2013
		snprintf(buffer, sizeof(buffer) - 1,
			"%s failed%s%s%s, errno = %i (%s)", expr_,
			msg_ ? " (additional info: " : "", msg_ ? msg_ : "",
			msg_ ? ")" : "", zmq_errno(), zmq_strerror(zmq_errno()));
		LOGFMTE("%d %s", line_, buffer);
	}
	return rc_;
}

#define TEST_ASSERT_SUCCESS_ERRNO(expr)                                        \
    test_assert_success_message_errno_helper (expr, NULL, #expr, __LINE__)




class ZeroServerCtxEx:public MQNotifier {
public:
	ZeroServerCtxEx() {
		m_zeroContext = zmq_ctx_new();
		zmq_ctx_set(m_zeroContext, ZMQ_IO_THREADS, 16);
		m_business = UDMSerBusiness::getInstance();
		m_business->addNotiify(this);
	}
	~ZeroServerCtxEx(){
		stop();
	}
	bool addPull(const string address) {
		m_pullAddressList.push_back(address);
		return true;
	}
	bool addPub(const string address) {
		if (pubSock)
		{
			LOGFMTE("publish socket had created before");
			return false;
		}
		pubSock = zmq_socket(m_zeroContext, ZMQ_PUB);
		if (TEST_ASSERT_SUCCESS_ERRNO(zmq_bind(pubSock, address.c_str())) == -1)
		{
			return false;
		}
		return true;
	}
	bool addRequest(const string address) {
		if (requestSock)
		{
			LOGFMTE("request socket had created before");
			return false;
		}
		requestSock = zmq_socket(m_zeroContext, ZMQ_REP);
		if (TEST_ASSERT_SUCCESS_ERRNO(zmq_bind(requestSock, address.c_str())) == -1)
		{
			return false;
		}
		return true;
	}
	bool start()
	{
		if (m_running == true)
			return true;
		//LOGFMTF("start recv&send thread\n");
		m_running = true;
		PollStart();
		//LOGFMTF("start recv&send thread success\n");
		return true;
	}
	bool stop() 
	{
		if (m_running == true)
		{
			//LOGFMTF("stop recv&send thread\n");
			m_running = false;
			pollThd->join();
			//LOGFMTF("stop recv&send thread success\n");
		}
		return true;
	}
public:
	void onCommonMessage(std::shared_ptr <UdmMessage> message) const override
	{
		int rc = -1;
		int msgDoneCount = 0;
		zmq_msg_t msg;

		if (pubSock == nullptr) 
		{
			LOGFMTT("udm have no publish role\n");
			return;
		}

		if (m_running == true) 
		{
			SEF_Size_t size = 0;
			const void* data = message->GetBytes(size);
			rc = zmq_msg_init_size(&msg, size);
			if (rc != 0) 
			{
				LOGFMTT("error in zmq_msg_init_size: %s\n",zmq_strerror(errno));
				return;
			}
			memcpy(zmq_msg_data(&msg), data, size);
			zmq_sendmsg(pubSock, &msg, sendFlags);

			if ((++msgDoneCount) % 50 == 0) 
			{
				LOGFMTT("udmservice current send message(%zu )%s\n", size, (char*)data);
			}
			zmq_msg_close(&msg);
		}
	}

private:
	void PollStart()
	{
		pollThd = new thread(&ZeroServerCtxEx::pull_thread_poll, this);
	}


	void pull_thread_poll()
	{
		int rc = -1;
		int i = 0;
		size_t size = m_pullAddressList.size();
		if (size == 0) 
		{
			return;
		}
		size += 1;
		zmq_pollitem_t* items = (zmq_pollitem_t*)malloc(size * sizeof(zmq_pollitem_t));
		memset((void *)items, 0, size * sizeof(zmq_pollitem_t));
		for (string addr: m_pullAddressList)
		{
			subSock = zmq_socket(m_zeroContext, ZMQ_PULL);
			/*TEST_ASSERT_SUCCESS_ERRNO(zmq_bind(subSock, addr.c_str()));*/
			zmq_bind(subSock, addr.c_str());
			items[i].events |= ZMQ_POLLIN;
			items[i].socket = subSock;
			items[i].fd = 0;
			items[i].revents = 0;
			i++;
		}
		items[i].events |= ZMQ_POLLIN;
		items[i].socket = requestSock;
		items[i].fd = 0;
		items[i].revents = 0;

		//zmq_pollitem_t items[] = {
		//	{subSock,0,ZMQ_POLLIN,0}
		//};
		LOGFMTT("recv_thread_poll start\n");
		while (m_running == true) 
		{
			rc = zmq_poll(items, size, 100);

			if (items[0].revents & ZMQ_POLLIN)
			{
				std::shared_ptr<UdmMessage >jsonMsg = std::make_shared<UdmMessage>();
				zmq_msg_t msg;
				zmq_msg_init(&msg);
				rc = zmq_recvmsg(items[0].socket, &msg, 0);
				if (rc < 0) 
				{
					LOGFMTE("error in zmq_recvmsg: %s\n", zmq_strerror(errno));
					continue;
				}

				if (jsonMsg->deserialize(zmq_msg_data(&msg), zmq_msg_size(&msg)) == false)
				{
					LOGFMTE("buf2json failed:%s", (char *)zmq_msg_data(&msg));
					zmq_msg_close(&msg);
					continue;
				}
				//LOGFMTT("service recv topic %s", jsonMsg->GetTopicName().c_str());
				m_business->onRecv(jsonMsg);
				zmq_msg_close(&msg);
			}
			else if (items[i].revents & ZMQ_POLLIN) 
			{
				UdmMessage jsonMsg;
				UdmMessage jsonResponse;
				zmq_msg_t msg;
				zmq_msg_init(&msg);
				rc = zmq_recvmsg(items[i].socket, &msg, 0);
				if (rc < 0) 
				{
					LOGFMTE("error in zmq_recvmsg: %s\n", zmq_strerror(errno));
					continue;
				}
				if (jsonMsg.deserialize(zmq_msg_data(&msg), zmq_msg_size(&msg)) == false) 
				{
					LOGFMTE("buf2json failed:%s", (char*)zmq_msg_data(&msg));
					zmq_msg_close(&msg);
					continue;
				}
				if (jsonMsg.GetMsgType() == ValueType::VALUE_TYPE_QUERY_TOPICNODE) 
				{
					zmq_msg_t reply;
					bool res = m_business->onRequestAllItems(jsonMsg, jsonResponse);
					if (res == true) 
					{
						SEF_Size_t size = 0;
						const void* data = jsonResponse.GetBytes(size);
						zmq_msg_init_size(&reply, size);
						memcpy(zmq_msg_data(&reply), data, size);
						zmq_sendmsg(items[i].socket, &reply, sendFlags);
					}
					else 
					{
						zmq_msg_init(&reply);
						zmq_sendmsg(items[i].socket, &reply, sendFlags);
					}
					zmq_msg_close(&reply);
				}
				else 
				{
					LOGFMTE("zeroMQ service recv unsupport value type(%d) at request socket", jsonMsg.GetMsgType());
				}
			}
		}
		LOGFMTF("send_thread_poll exit\n");
	}

public:
	void* m_zeroContext = nullptr;
	void* pubSock = nullptr;
	void* subSock = nullptr;
	void* requestSock = nullptr;
	void* poller = nullptr;
	std::thread* pollThd = nullptr;

	UDMSerBusiness* m_business = nullptr;

	list<string> m_pullAddressList;
	list<string> m_pubAddressList;

	bool m_running;
};


ZeroMQServiceEx::ZeroMQServiceEx()
{
}

ZeroMQServiceEx::~ZeroMQServiceEx()
{
}

bool ZeroMQServiceEx::init(const char* config)
{
	if (m_ctx == nullptr)
	{
		ZeroInit();
	}
	return false;
}

bool ZeroMQServiceEx::init( unsigned int frequency, unsigned int timeout)
{
	m_frequency = frequency;
	m_timeout = timeout;
	if (m_ctx == nullptr)
	{
		ZeroInit();
	}
	return true;
}

bool ZeroMQServiceEx::addAddress(const UdmAddress& info)
{
	string pullUrl;
	string pubUrl;
	string requestUrl;
	bool ret = true;
	SEF_Ushort basePort = info.port;
	if (m_ctx == nullptr)
	{
		LOGFMTE("error address info, socket content is null");
		return false;
	}

	if ((ProtocolType)info.protocol == ProtocolType::PROTOCOL_TYPE_TCP)
	{
		pullUrl = "tcp://" + info.ip + ":" + to_string(basePort);
		basePort += portStep;
		pubUrl = "tcp://" + info.ip + ":" + to_string(basePort);
		basePort += portStep;
		requestUrl = "tcp://" + info.ip + ":" + to_string(basePort);
	}
	else
	{
		LOGFMTE("error address info, unsupport protocol type:%d", info.protocol);
		return false;
	}
	ret &= m_ctx->addPull(pullUrl);
	ret &= m_ctx->addPub(pubUrl);
	ret &= m_ctx->addRequest(requestUrl);
	return ret;
}

bool ZeroMQServiceEx::start()
{
	return m_ctx->start();
}

void ZeroMQServiceEx::stop()
{

}

bool ZeroMQServiceEx::ZeroInit()
{
	m_ctx = new ZeroServerCtxEx();
	return true;
}
