#include "RequestQueueIntf.h"
#include "log.h"
using namespace SEF::Element;
using namespace SEF::COMMON;

RequestQueueIntf::RequestQueueIntf()
{

}

RequestQueueIntf::~RequestQueueIntf()
{
}

void RequestQueueIntf::registerQueue(const SEF_String& topic)
{
	QueueNameMap* queueNameMap = nullptr;
	auto it = m_requestQueueSet.find(topic);
	if (it == m_requestQueueSet.end())
	{
		queueNameMap = new QueueNameMap();

		RequestQueueData* defaultQueueData = new RequestQueueData();
		defaultQueueData->queue = new RequestQueue(s_defaultQueueName);
		queueNameMap->insert(std::make_pair(s_defaultQueueName, defaultQueueData));
		m_requestQueueSet.insert(std::make_pair(topic, queueNameMap));
		LOGFMTD("new queue register to new topic map success: topic:%s queue:%s", topic.c_str(), s_defaultQueueName);
	}
}

void RequestQueueIntf::registerQueue(const SEF_String& topic, const SEF_String& queueName)
{
	QueueNameMap* queueNameMap = nullptr;
	auto it = m_requestQueueSet.find(topic);
	if(it == m_requestQueueSet.end())
	{
		queueNameMap = new QueueNameMap();

		RequestQueueData* requestQueueData = new RequestQueueData();
		requestQueueData->queue = new RequestQueue(queueName);
		queueNameMap->insert(std::make_pair(queueName, requestQueueData));

		RequestQueueData* defaultQueueData = new RequestQueueData();
		defaultQueueData->queue = new RequestQueue(s_defaultQueueName);
		queueNameMap->insert(std::make_pair(s_defaultQueueName, defaultQueueData));

		m_requestQueueSet.insert(std::make_pair(topic, queueNameMap));
		LOGFMTD("new queue register to new topic map success: topic:%s queue:%s ", topic.c_str(), queueName.c_str());
	}
	else
	{
		queueNameMap = it->second;
		const auto & its = queueNameMap->find(queueName);
		if (its == queueNameMap->end())
		{
			RequestQueueData* requestQueueData = new RequestQueueData();
			requestQueueData->queue = new RequestQueue(queueName);
			queueNameMap->insert(make_pair(queueName, requestQueueData));
			LOGFMTD("new queue register to old topic map success: topic:%s queue:%s", topic.c_str(), queueName.c_str());
		}
		else
		{
			LOGFMTD(" queue register to old topic map failed: topic:%s queue:%s", topic.c_str(), queueName.c_str());
		}
	}
}

void RequestQueueIntf::registerQueueHandler(const SEF_String& topic, const SEF_String& queueName, const SEF_String& requestName, RequestHandler handler)
{
	RequestQueueData* requestQueueData = match(topic, queueName);
	if (requestQueueData)
	{
		requestQueueData->handlers.insert(make_pair(requestName, handler));
		LOGFMTD("request handle register success: topic:%s queue:%s name: %s", topic.c_str(), queueName.c_str(), requestName.c_str());
	}
	else
	{
		LOGFMTD("request handle register failed: topic:%s queue:%s name: %s", topic.c_str(), queueName.c_str(), requestName.c_str());
	}
	registerRequest(topic, queueName, requestName);
}

void RequestQueueIntf::registerQueueHandler(const SEF_String& topic, const SEF_String& requestName, RequestHandler handler)
{
	RequestQueueData* requestQueueData = match(topic, s_defaultQueueName);
	if (requestQueueData)
	{
		requestQueueData->handlers.insert(make_pair(requestName, handler));
		LOGFMTD("request handle register success: topic:%s queue:%s name: %s", topic.c_str(), s_defaultQueueName, requestName.c_str());
	}
	registerRequest(topic, s_defaultQueueName, requestName);
}

void RequestQueueIntf::requestQueuePopped(const SEF_String& topic, const SEF_String& queueName, RequestQueue* queue, const Request& request, const SEF_Bool handled)
{
}

void RequestQueueIntf::requestQueuePushed(const SEF_String& topic, const SEF_String& queueName, RequestQueue* queue, const Request& request)
{
}

void RequestQueueIntf::setName(const SEF_String& name)
{
	m_name = name;
}

SEF_Bool SEF::Element::RequestQueueIntf::handleRequestQueues(SEF_Bool handleAll)
{
	SEF_Int size = 0;
	for (const auto &it: m_requestQueueSet)
	{
		SEF_String topic = it.first;
		QueueNameMap* queueNameMap = it.second;
		for (const auto &qIt: *queueNameMap)
		{
			SEF_String queueName = qIt.first;
			RequestQueueData* requestQueueData = qIt.second;
			size+=handleRequestQueueData(topic, requestQueueData, handleAll);
		}
	}
	if (size > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void RequestQueueIntf::handleRequestQueue(const SEF_String& topicName, const SEF_String& queueName, const SEF_Bool handleAll)
{
	RequestQueueData* requestQueueData = match(topicName, queueName);
	handleRequestQueueData(topicName, requestQueueData, handleAll);
}

SEF_Int RequestQueueIntf::handleRequestQueueData(const SEF_String& topicName, RequestQueueData* requestQueueData, SEF::COMMON::SEF_Bool handleAll)
{
	SEF_Int size = 0;
	if (requestQueueData)
	{
		RequestQueue newQueue = std::move(*requestQueueData->queue);
		//if (newQueue.getSize() > 0)
		//{
		//	LOGFMTD("request handle register success: topic:%s queue:%s   queue size: %d(handle size %lu)",
		//		topicName.c_str(), newQueue.getName().c_str(), newQueue.getSize(), requestQueueData->handlers.size());
		//}

		if (handleAll == true)
		{
			std::shared_ptr<Request> request;
			while (newQueue.popFront(request))
			{
				auto it = requestQueueData->handlers.find(request->getName());
				if (it != requestQueueData->handlers.end())
				{
					RequestQueueArgs arg;
					arg.queueName = newQueue.getName();
					arg.topicName = topicName;
					arg.request = request;
					(this->*(it->second))(arg);
					size++;
				}
			}
		}
		else
		{
			std::map<SEF_String, Request> map;
			std::shared_ptr<Request> request;
			while (newQueue.popFront(request) == true)
			{
				auto it = requestQueueData->handlers.find(request->getName());
				if (it != requestQueueData->handlers.end())
				{
					RequestQueueArgs arg;
					arg.queueName = newQueue.getName();
					arg.topicName = topicName;
					arg.request = request;
					(this->*(it->second))(arg);
					size++;
				}
			}
		}
	}
	return size;
}

void RequestQueueIntf::clearQueue(const SEF_String& topic, const SEF_String& queueName)
{
	RequestQueueData* requestQueueData = match(topic, queueName);
	requestQueueData->queue->lock();
	requestQueueData->queue->clear();
	requestQueueData->queue->unlock();
}

void RequestQueueIntf::clearAllQueues()
{
	for (const auto &it: m_requestQueueSet)
	{
		QueueNameMap* map = it.second;
		for (const auto &its:*map)
		{
			RequestQueueData* requestQueueData = its.second;
			requestQueueData->queue->lock();
			requestQueueData->queue->clear();
			requestQueueData->queue->unlock();
		}
	}
}

SEF::COMMON::SEF_Bool SEF::Element::RequestQueueIntf::pushRequest(const SEF::COMMON::SEF_String& topicName, const SEF::COMMON::SEF_String& queueName, const SEF::COMMON::ByteArray& requestBytes)
{
	RequestQueueData* queueData = match(topicName, queueName);
	if (queueData == nullptr)
	{
		LOGFMTD("request match failed:topic:%s queue:%s", topicName.c_str(), queueName.c_str());
		return false;
	}
	std::shared_ptr<Request> request = std::make_shared<Request>();
	request->deserialize(requestBytes);
	queueData->queue->lock();
	queueData->queue->pushBack(request);
	queueData->queue->unlock();
	if (queueData->queue->getSize()>2000&&queueData->queue->getSize()% 100>50)
	{
		LOGFMTD("queue add request: topic:%s queue:%s  size %lu", topicName.c_str(), queueName.c_str(), queueData->queue->getSize());
	}
 	return true;
}

SEF::COMMON::SEF_Bool SEF::Element::RequestQueueIntf::pushRequest(const SEF::COMMON::SEF_String& topicName, const SEF::COMMON::ByteArray& requestBytes)
{
	RequestQueueData* queueData = match(topicName, s_defaultQueueName);
	if (queueData == nullptr)
	{
		return false;
	}
	std::shared_ptr<Request> request = std::make_shared<Request>();
	request->deserialize(requestBytes);
	queueData->queue->lock();
	queueData->queue->pushBack(request);
	queueData->queue->unlock();
	if (queueData->queue->getSize() > 2000 && queueData->queue->getSize() % 100 > 50)
	{
		LOGFMTD("topic:%s queue:%s  size %lu", topicName.c_str(), s_defaultQueueName, queueData->queue->getSize());
	}
	return true;
}

RequestQueueIntf::QueueNameMap* RequestQueueIntf::match(const SEF::COMMON::SEF_String& topic)
{
	auto it = m_requestQueueSet.find(topic);
	if (it == m_requestQueueSet.end())
	{
		return nullptr;
	}
	return it->second;
}

RequestQueueIntf::RequestQueueData* RequestQueueIntf::match(const SEF::COMMON::SEF_String& topic, const SEF::COMMON::SEF_String& queueName)
{
	QueueNameMap* queueNameMap = match(topic);
	if (queueNameMap)
	{
		auto it = queueNameMap->find(queueName);
		if (it == queueNameMap->end())
		{
			LOGFMTD("match failed: topic:%s queue:%s", topic.c_str(), queueName.c_str());
			return nullptr;
		}
		return it->second;
	}
	LOGFMTD("match failed: topic:%s queue:%s", topic.c_str(), queueName.c_str());
	return nullptr;
}


