#include "ZeroMQUdmEx.h"
#include "zmq.h"
#include "UdmJsonRefMsg.h"
#include "SafeQueue.h"
#include <unordered_map>
#include <map>
#include "log.h"
#include "topicList.pb.h"
#include "internal.h"
#include <chrono>
#include <thread>
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using SEF::Udm::ZeroMQUdmEx;

using SEF::Udm::Udm;
using SEF::Udm::RequestCb;
using namespace SEF::COMMON;
using namespace SEF::Udm;
using namespace SEF::Config;
using namespace SEF::Log;
using namespace std;
using namespace UDM;
using namespace SEF::INTERNAL;

constexpr int sendFlags = ZMQ_NOBLOCK;// ZMQ_NOBLOCK | ZMQ_SNDMORE;

#define ZM_SAFE_CLOSE(ctx) do{  \
								if(ctx) zmq_close(ctx);			\
								ctx = nullptr;					\
							}while(0);							\

static SEF_Uint64 getCurrentSec()
{
	return time(nullptr);
}

static SEF_Uint64 UdmRand()
{
	std::time_t currentTime = std::time(nullptr);
	std::tm* localTime = std::localtime(&currentTime);

	auto now = std::chrono::system_clock::now();
	std::chrono::system_clock::duration duration = now.time_since_epoch();
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration);
	duration -= sec;
	auto millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	char buffer[80] = { 0 };
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", localTime);
	string day(buffer);
	std::srand(static_cast<unsigned int>(std::time(nullptr) + millseconds.count()));
	int random = std::rand();
	return random;
}
/*******************************************************/
static int
receive_monitor_address(void* monitor_, char** address_, bool expect_more_)
{
	zmq_msg_t msg;

	zmq_msg_init(&msg);
	if (zmq_msg_recv(&msg, monitor_, 0) == -1)
		return -1; //  Interrupted, presumably
	zmq_msg_more(&msg);

	if (address_) {
		const uint8_t* const data =
			static_cast<const uint8_t*> (zmq_msg_data(&msg));
		const size_t size = zmq_msg_size(&msg);
		*address_ = static_cast<char*> (malloc(size + 1));
		memcpy(*address_, data, size);
		(*address_)[size] = 0;
	}
	zmq_msg_close(&msg);

	return 0;
}

static int get_monitor_event_internal(void* monitor_,
	int* value_,
	char** address_,
	int recv_flag_)
{
	//  First frame in message contains event number and value
	zmq_msg_t msg;
	int rc = 0;
	zmq_msg_init(&msg);
	if (zmq_msg_recv(&msg, monitor_, recv_flag_) == -1) {
		return -1; //  timed out or no message available
	}
	rc = zmq_msg_more(&msg);

	uint8_t* data = static_cast<uint8_t*> (zmq_msg_data(&msg));
	uint16_t event = *reinterpret_cast<uint16_t*> (data);
	if (value_)
		memcpy(value_, data + 2, sizeof(uint32_t));

	//  Second frame in message contains event address
	receive_monitor_address(monitor_, address_, false);

	return event;
}

int get_monitor_event_with_timeout(void* monitor_, int* value_, char** address_, int timeout_)
{
	int res;
	if (timeout_ == -1) 
	{
		// process infinite timeout in small steps to allow the user
		// to see some information on the console

		int timeout_step = 250;
		int wait_time = 0;
		zmq_setsockopt(monitor_, ZMQ_RCVTIMEO, &timeout_step,
			sizeof(timeout_step));
		while (
			(res = get_monitor_event_internal(monitor_, value_, address_, 0))
			== -1) {
			wait_time += timeout_step;
			fprintf(stderr, "Still waiting for monitor event after %i ms\n",
				wait_time);
		}
	}
	else 
	{
		zmq_setsockopt(monitor_, ZMQ_RCVTIMEO, &timeout_, sizeof(timeout_));
		res = get_monitor_event_internal(monitor_, value_, address_, 0);
	}
	int timeout_infinite = -1;
	zmq_setsockopt(monitor_, ZMQ_RCVTIMEO, &timeout_infinite,
		sizeof(timeout_infinite));
	return res;
}

int get_monitor_event(void* monitor_, int* value_, char** address_)
{
	return get_monitor_event_with_timeout(monitor_, value_, address_, -1);
}
/**********************************/

class CtxControl {
public:
	static CtxControl& getInstance()
	{
		std::call_once(initFlag, [] {
			gInstance.reset(new CtxControl);
			});
		return *gInstance;
	}
	void push(void *ctx)
	{
		m_queue.push(ctx);
	}
	void cycle()
	{
		void* ctx = nullptr;
		while (m_queue.pop(ctx))
		{
			LOGFMTI("---------Start free zmq context---------");
			zmq_ctx_term(ctx);
			LOGFMTI("----------End free zmq context----------");
		}
	}
	~CtxControl()
	{
		m_queue.terminate();
	}
private:
	CtxControl()
	{
		m_th = new thread(&CtxControl::cycle, this);
	}


	CtxControl(const CtxControl&) = delete;
	CtxControl& operator=(const CtxControl&) = delete;

private:

	std::thread* m_th = nullptr;
	SafeQueue<void*> m_queue;

	static std::unique_ptr<CtxControl> gInstance;
	static std::once_flag initFlag;
};

std::unique_ptr<CtxControl> CtxControl::gInstance;
std::once_flag CtxControl::initFlag;

/******************************************************/
class ZeroContentEx {
	enum class RecvStat {
		RECV_STOP = 0,
		RECV_START = 1,
		RECV_PAUSE = 2,
	};
public:
	ZeroContentEx(list<UdmAddress>& config)
	{
		string pullUrl;
		string pubUrl;
		string requestUrl;
		SEF_Ushort basePort = 0;

		m_zeroContext = zmq_ctx_new();
		zmq_ctx_set(m_zeroContext, ZMQ_IO_THREADS, 2);

		SEF_Uint64 randId = UdmRand();
		m_mointorPushUrl = "inproc://monitor_push_" + to_string(randId) + ".sock";
		m_mointorSubUrl = "inproc://monitor_sub_" + to_string(randId) + ".sock";
		m_mointorRequestUrl = "inproc://monitor_request_" + to_string(randId) + ".sock";

		for (UdmAddress info : config) {
			basePort = info.port;
			if ((ProtocolType)info.protocol != ProtocolType::PROTOCOL_TYPE_TCP)
			{
				LOGFMTE("error address info, unsupport protocol type:%d", info.protocol);
				continue;
			}
			m_connected = PushCtxInit("tcp://" + info.ip + ":" + to_string(basePort), m_mointorPushUrl);
			if (m_connected == SEF_FALSE)
			{
				break;
			}
			basePort += portStep;
			m_connected = SubCtxInit("tcp://" + info.ip + ":" + to_string(basePort), m_mointorSubUrl);
			if (m_connected == SEF_FALSE)
			{
				break;
			}
			basePort += portStep;
			m_connected = RequestCtxInit("tcp://" + info.ip + ":" + to_string(basePort), m_mointorRequestUrl);
			if (m_connected == SEF_FALSE)
			{
				break;
			}
			LOGFMTD(" zeromq udm client set address sucessfully,%s:%d", info.ip.c_str(), info.port);
			m_connected = SEF_TRUE;
			break;
		}
	}
	~ZeroContentEx()
	{
		m_isRunning = false;
		if (pubSock)
		{
			ZM_SAFE_CLOSE(pubSock);
			pubSock = nullptr;
		}
		if (subSock) 
		{
			ZM_SAFE_CLOSE(subSock);
			subSock = nullptr;
		}
		if (requestSock)
		{
			ZM_SAFE_CLOSE(requestSock);
			requestSock = nullptr;
		}
		if (MonitorPubSock)
		{
			ZM_SAFE_CLOSE(MonitorPubSock);
			MonitorPubSock = nullptr;
		}
		if (MonitorRequestSock)
		{
			ZM_SAFE_CLOSE(MonitorRequestSock);
			MonitorRequestSock = nullptr;
		}
		if (MonitorSubSock)
		{
			ZM_SAFE_CLOSE(MonitorSubSock);
			MonitorSubSock = nullptr;
		}
		LOGFMTI("---------Start term zmq context---------");
		CtxControl& ins = CtxControl::getInstance();
		ins.push(m_zeroContext);
		m_zeroContext = nullptr;
		LOGFMTI("----------End term zmq context----------");
	}
	void setSocketOpt(void *socket, int tcpKeepalive, int keepaliveIdleTime) 
	{
		//zmq_setsockopt(socket,ZMQ_TCP_KEEPALIVE, &tcpKeepalive, sizeof(int));
		//zmq_setsockopt(socket, ZMQ_TCP_KEEPALIVE_IDLE, &keepaliveIdleTime, sizeof(int));
	}
	void setSendWindows(void* socket)
	{
		int hwm = 1000;
		zmq_setsockopt(socket,ZMQ_SNDHWM, &hwm, sizeof(hwm));
	}
	void setSendNonblock(void* socket)
	{
		int nonblock = 1;
		zmq_setsockopt(socket, ZMQ_NOBLOCK, &nonblock, sizeof(nonblock));
	}
	SEF_Bool PushCtxInit(const string& url, const string& mointorUrl)
	{
		if (pubSock) 
		{
			return SEF_TRUE;
		}
		pubSock = zmq_socket(m_zeroContext, ZMQ_PUSH);
		if (!pubSock)
		{
			printf("error in zmq_socket: %s\n", zmq_strerror(errno));
			return SEF_FALSE;
		}
		setSocketOpt(pubSock, 1, 180);
		setSendNonblock(pubSock);
		int rc = zmq_connect(pubSock, url.c_str());
		if (rc != 0) 
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(pubSock);
			return SEF_FALSE;
		}
		rc = zmq_socket_monitor(pubSock, mointorUrl.c_str(), ZMQ_EVENT_ALL);
		if (rc != 0)
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(pubSock);
			return SEF_FALSE;
		}
		LOGT("push connect:" + url + " success");
		return SEF_TRUE;
	}
	SEF_Bool SubCtxInit(const string &url, const string& mointorUrl)
	{
		if (subSock) 
		{
			return SEF_TRUE;
		}

		subSock = zmq_socket(m_zeroContext, ZMQ_SUB);
		if (!subSock)
		{
			printf("error in zmq_socket: %s\n", zmq_strerror(errno));
			return SEF_FALSE;
		}
		setSocketOpt(subSock, 1, 180);
		int rc = zmq_connect(subSock, url.c_str());
		if (rc != 0)
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(subSock);
			return SEF_FALSE;
		}
		rc = zmq_socket_monitor(subSock, mointorUrl.c_str(), ZMQ_EVENT_ALL);
		if (rc != 0)
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(subSock);
			return SEF_FALSE;
		}
		LOGT("subscribe connect:"+url+" success");
		return SEF_TRUE;
	}
	SEF_Bool RequestCtxInit(const string& url, const string& mointorUrl)
	{
		if (requestSock)
		{
			return SEF_TRUE;
		}

		requestSock = zmq_socket(m_zeroContext, ZMQ_REQ);
		if (!requestSock) 
		{
			printf("error in zmq_socket: %s\n", zmq_strerror(errno));
			return SEF_FALSE;
		}
		setSendNonblock(requestSock);
		int rc = zmq_connect(requestSock, url.c_str());
		if (rc != 0) 
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(requestSock);
			return SEF_FALSE;
		}
		rc = zmq_socket_monitor(requestSock, mointorUrl.c_str(), ZMQ_EVENT_ALL);
		if (rc != 0)
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(requestSock);
			return SEF_FALSE;
		}
		int timeout = 5000;
		rc = zmq_setsockopt(requestSock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
		assert(rc == 0);
		LOGT("subscribe connect:" + url + " success");
		return SEF_TRUE;
	}
	void PollStart()
	{
		if (m_isRunning == true)
		{
			return;
		}
		m_isRunning = true;
		stat = RecvStat::RECV_START;
	}
	bool isRunning()
	{
		return m_isRunning;
	}
	bool isConnected()
	{
		//return m_isConnected;
		return true;
	}
	bool publish(const void *buf, size_t message_size)
	{
		if (pubSock == nullptr||message_size==0)
		{
			return false;
		}
		zmq_msg_t msg;
		int rc = zmq_msg_init_size(&msg, message_size);
		if (rc != 0)
		{
			LOGFMTT("error in zmq_msg_init_size: %s\n", zmq_strerror(zmq_errno()));
			return false;
		}
		memcpy(zmq_msg_data(&msg), buf, message_size);
		rc = zmq_sendmsg(pubSock, &msg, sendFlags);
		if (rc == -1)
		{
			LOGFMTT("error in send msg: %s\n", zmq_strerror(zmq_errno()));
			zmq_msg_close(&msg);
			return false;
		}
		publishCount++;
		if (publishCount++ % 50 == 0) 
		{
			LOGFMTT("(top pub msg count %lu) current publish message: %s\n", publishCount,(char*)buf);
		}
		zmq_msg_close(&msg);
		return true;
	}
	SEF_Bool startMonitor()
	{
		MonitorPubSock = zmq_socket(m_zeroContext, ZMQ_PAIR);
		if (!MonitorPubSock)
		{
			printf("error in zmq_socket: %s\n", zmq_strerror(errno));
			return SEF_FALSE;
		}
		int rc = zmq_connect(MonitorPubSock, m_mointorPushUrl.c_str());
		if (rc != 0)
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(MonitorPubSock);
			return SEF_FALSE;
		}

		MonitorSubSock = zmq_socket(m_zeroContext, ZMQ_PAIR);
		if (!MonitorSubSock)
		{
			printf("error in zmq_socket: %s\n", zmq_strerror(errno));
			return SEF_FALSE;
		}
		rc = zmq_connect(MonitorSubSock, m_mointorSubUrl.c_str());
		if (rc != 0)
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(MonitorSubSock);
			return SEF_FALSE;
		}

		MonitorRequestSock = zmq_socket(m_zeroContext, ZMQ_PAIR);
		if (!MonitorRequestSock)
		{
			printf("error in zmq_socket: %s\n", zmq_strerror(errno));
			return SEF_FALSE;
		}
		rc = zmq_connect(MonitorRequestSock, m_mointorRequestUrl.c_str());
		if (rc != 0)
		{
			printf("error in zmq_connect: %s\n", zmq_strerror(errno));
			ZM_SAFE_CLOSE(MonitorRequestSock);
			return SEF_FALSE;
		}
		return SEF_TRUE;
	}

	UdmEventType zmqEventToUdm(int type)
	{
		switch (type)
		{
		case ZMQ_EVENT_CONNECTED:
		{
			m_isConnected = true;
			//LOGFMTE("\nudm connected(zmq)\n");
			return UdmEventType::UDM_EVENT_CONNECTED;
		}
		case ZMQ_EVENT_CONNECT_DELAYED: return UdmEventType::UDM_EVENT_CONNECTED_DELAY;
		case ZMQ_EVENT_CONNECT_RETRIED: return UdmEventType::UDM_EVENT_CONNECT_RETRIED;
		case ZMQ_EVENT_DISCONNECTED: 
		case ZMQ_EVENT_CLOSED: {
			m_isConnected = false;
			//LOGFMTE("\nudm disconnected(zmq)\n");
			return UdmEventType::UDM_EVENT_DISCONNECTED;
		}
		default:return UdmEventType::UDM_EVENT_NONE;
		}
	}

public:
	void* m_zeroContext = nullptr;
	void* pubSock = nullptr;
	void* subSock = nullptr;
	void* requestSock = nullptr;
	SEF_String m_mointorPushUrl;
	SEF_String m_mointorSubUrl;
	SEF_String m_mointorRequestUrl;
	void* MonitorPubSock = nullptr;
	void* MonitorSubSock = nullptr;
	void* MonitorRequestSock = nullptr;
	SEF_Bool m_connected = false;
	SEF_Uint64 publishCount = 0;
	bool m_isRunning;
	RecvStat  stat = RecvStat::RECV_STOP;
	bool	m_isConnected = false;
};

ZeroMQUdmEx::ZeroMQUdmEx()
{
	udm_type = UdmType::ZeroMQ;
}

ZeroMQUdmEx::~ZeroMQUdmEx() 
{
	m_ctx->m_isRunning = false;
	if (m_thread&&m_thread->joinable())
	{
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
	}
	if (m_MonitorThread && m_MonitorThread->joinable())
	{
		m_MonitorThread->join();
		delete m_MonitorThread;
		m_MonitorThread = nullptr;
	}
	delete m_ctx;
	m_ctx = nullptr;
}

bool ZeroMQUdmEx::init()
{
	//LOGFMTI("\n---------Start term zmq---------\n");
	bool res = ZeroContextInit();
	//LOGFMTI("\n---------End term zmq---------res %d\n", res);
	return res;
}

bool SEF::Udm::ZeroMQUdmEx::publish(std::shared_ptr<UdmMessage> udmMessage)
{
	SEF_Size_t size = 0;
	const void* data = udmMessage->GetBytes(size);
	return m_ctx->publish(data, size);
}

bool SEF::Udm::ZeroMQUdmEx::_sendRequest(UdmMessage& message, UdmMessage& reply)
{
	int rc = -1;
	SEF_Size_t size = 0;
	const void* data = message.GetBytes(size);
	zmq_msg_t msg;
	zmq_msg_t msgReply;

	if (m_ctx->requestSock == nullptr)
	{
		LOGFMTE("udm request socket not exist");
		return false;
	}

	rc = zmq_msg_init_size(&msg, size);
	if (rc != 0) 
	{
		LOGFMTT("error in zmq_msg_init_size: %s\n", zmq_strerror(errno));
		return false;
	}
	memcpy(zmq_msg_data(&msg), data, size);
	rc = zmq_sendmsg(m_ctx->requestSock, &msg, sendFlags);
	if (rc < 0)
	{
		LOGFMTT("error in send: %s\n", zmq_strerror(errno));
		return false;
	}
	zmq_msg_close(&msg);

	zmq_msg_init(&msgReply);
	rc = zmq_recvmsg(m_ctx->requestSock, &msgReply, 0);
	if (rc < 0)
	{
		LOGFMTT("error in zmq_recvmsg: %s\n", zmq_strerror(errno));
		return false;
	}
	void* msgData = zmq_msg_data(&msgReply);
	size_t  msgSize = zmq_msg_size(&msgReply);
	if (reply.deserialize(msgData, msgSize) == true) 
	{
		zmq_msg_close(&msgReply);
		return true;
	}
	return false;
}

bool SEF::Udm::ZeroMQUdmEx::subscribe(const string& topic)
{
	if (m_ctx->subSock == nullptr)
	{
		return false;
	}
	if (topic.empty() == false)
	{
		int rc = zmq_setsockopt(m_ctx->subSock, ZMQ_SUBSCRIBE, topic.c_str(), strlen(topic.c_str()));
		if (rc <= -1) 
		{
			return false;
		}
	}
	else
	{
		int rc = zmq_setsockopt(m_ctx->subSock, ZMQ_SUBSCRIBE, "", 0);
		if (rc <= -1) 
		{
			return false;
		}
	}
	return true;
}

bool SEF::Udm::ZeroMQUdmEx::unsubscribe(const string& topic)
{
	if (m_ctx->subSock == nullptr)
	{
		return false;
	}
	if (topic.empty() == false)
	{
		int rc = zmq_setsockopt(m_ctx->subSock, ZMQ_UNSUBSCRIBE, topic.c_str(), strlen(topic.c_str()));
		if (rc <= -1)
		{
			return false;
		}
	}
	else
	{
		int rc = zmq_setsockopt(m_ctx->subSock, ZMQ_UNSUBSCRIBE, "", 0);
		if (rc <= -1)
		{
			return false;
		}
	}
	return true;
}


bool SEF::Udm::ZeroMQUdmEx::pushSubItem(UdmMessage& udmMessage)
{
	SEF_Size_t size = 0;
	const void* data = udmMessage.GetBytes(size);
	return m_ctx->publish(data, size);
}

void SEF::Udm::ZeroMQUdmEx::setEventCb(UdmEventCb* cb)
{
	p_eventCb = cb;
}

bool SEF::Udm::ZeroMQUdmEx::isUdmConnected()
{
	return m_ctx->isConnected();
}


bool ZeroMQUdmEx::ZeroContextInit()
{
	if (m_ctx == nullptr) 
	{
		m_ctx = new ZeroContentEx(udm_address);
		m_ctx->PollStart();
		m_thread = new thread(&ZeroMQUdmEx::zerothreadCycle, this);
		if (m_ctx->m_connected == true)
		{
			m_ctx->startMonitor();
			m_MonitorThread = new thread(&ZeroMQUdmEx::MonitorCycle, this);
		}
	}
	return m_ctx->m_connected;
}

void ZeroMQUdmEx::zerothreadCycle()
{
	int rc = -1;
	zmq_pollitem_t items[] = 
	{
		{m_ctx->subSock,0,ZMQ_POLLIN,0}
	};
	while (m_ctx->m_isRunning == true&& m_ctx->subSock)
	{
		rc = zmq_poll(items, 1, 100);
		if (items[0].revents & ZMQ_POLLIN)
		{
			std::shared_ptr<UdmMessage >udmMessage = std::make_shared<UdmMessage>();
			zmq_msg_t msg;
			zmq_msg_init(&msg);
			rc = zmq_recvmsg(items[0].socket, &msg, 0);
			if (rc < 0) 
			{
				printf("error in zmq_recvmsg: %s\n", zmq_strerror(errno));
				continue;
			}
			void* data = zmq_msg_data(&msg);
			size_t  size = zmq_msg_size(&msg);
			if (udmMessage->deserialize(data, size) == true) 
			{
				onRecv(udmMessage);
			}
			zmq_msg_close(&msg);
		}
	}
}

void ZeroMQUdmEx::MonitorCycle()
{
	int rc = -1;
	zmq_pollitem_t items[] =
	{
		{m_ctx->MonitorPubSock,0,ZMQ_POLLIN,0},
		{m_ctx->MonitorSubSock,0,ZMQ_POLLIN,0},
		{m_ctx->MonitorRequestSock,0,ZMQ_POLLIN,0}

	};
	while (m_ctx->m_isRunning == true && m_ctx->subSock)
	{
		int event = 0;
		rc = zmq_poll(items, 3, 100);
		if (items[0].events & ZMQ_POLLIN)
		{
			event = get_monitor_event_with_timeout(m_ctx->MonitorPubSock, nullptr, nullptr, 5000);
			UdmEventType type = m_ctx->zmqEventToUdm(event);
			if (p_eventCb&& type!= UdmEventType::UDM_EVENT_NONE)
			{
				p_eventCb->OnEventRecv(type);
				LOGFMTD("%s event %d", m_ctx->m_mointorPushUrl.c_str(), type);

			}
		}
		else if(items[1].events & ZMQ_POLLIN)
		{
			event = get_monitor_event_with_timeout(m_ctx->MonitorSubSock, nullptr, nullptr, 5000);
			UdmEventType type = m_ctx->zmqEventToUdm(event);
			if (p_eventCb && type != UdmEventType::UDM_EVENT_NONE)
			{
				p_eventCb->OnEventRecv(type);
				LOGFMTD("%s event %d", m_ctx->m_mointorSubUrl.c_str(), type);
			}
		}
		else if (items[2].events & ZMQ_POLLIN)
		{
			event = get_monitor_event_with_timeout(m_ctx->MonitorRequestSock, nullptr, nullptr, 5000);
			UdmEventType type = m_ctx->zmqEventToUdm(event);
			if (p_eventCb && type != UdmEventType::UDM_EVENT_NONE)
			{
				p_eventCb->OnEventRecv(type);
				LOGFMTD("%s event %d", m_ctx->m_mointorRequestUrl.c_str(),type);
			}
		}
	}
	LOGFMTD("%s mointor thread exit", m_ctx->m_mointorPushUrl.c_str());
}


