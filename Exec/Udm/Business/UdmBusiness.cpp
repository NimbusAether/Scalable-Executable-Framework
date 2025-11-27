#include "SafeQueue.h"
#include <unordered_map>
#include <map>
#include "log.h"
#include "topicList.pb.h"
#include "internal.h"
#include "UdmBusiness.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using SEF::Udm::UdmBusiness;
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

static inline string  GenrateMapKey(const string& topic, const string& varName, const string& varUnit)
{
	return topic + ":" + varName + ":" + varUnit;
}


static void buildVarItemFromMsg(std::vector<MessageInfo>& vec, topicList& list)
{
	int nodeSize = list.nodes_size();
	if (nodeSize <= 0)
	{
		LOGFMTE("empty topic Items, node size %lu", nodeSize);
		return;
	}
	for (int i = 0; i < nodeSize; i++)
	{
		Node node = list.nodes(i);
		MessageInfo info;
		info.topicName = node.topicname();
		SEF_String param = node.name();
		size_t start = 0;
		size_t end = param.find(SEF::INTERNAL::ITEM_SPLIC_CHAR);
		info.variableName = param.substr(start, end);
		info.variableUnit = param.substr(end + 1, param.size());
		info.type = static_cast<ValueType>(node.type());
		vec.push_back(info);
	}
}

static void buildReqItemFromMsg(std::vector<RequestInfo>& vec, topicList& list)
{
	int nodeSize = list.nodes_size();
	if (nodeSize <= 0)
	{
		LOGFMTE("empty topic Items, node size %lu", nodeSize);
		return;
	}
	for (int i = 0; i < nodeSize; i++)
	{
		Node node = list.nodes(i);
		RequestInfo info;
		info.topicName = node.topicname();
		SEF_String param = node.name();
		size_t start = 0;
		size_t end = param.find(SEF::INTERNAL::ITEM_SPLIC_CHAR);
		info.queueName = param.substr(start, end);
		info.requestName = param.substr(end + 1, param.size());
		vec.push_back(info);
	}
}

class BusContent {
public:
	BusContent():m_enable(true){}
	~BusContent(){}

public:
	/*
	* m_manualQueue 手动更新UdmMessage
	*/
	SafeQueue<std::shared_ptr<UdmMessage>> m_manualQueue;
	bool	m_manualQueueEnable = true;
	SafeQueue<std::shared_ptr<UdmMessage>> m_notifyQueue;
	std::mutex m_mapMutex;
	std::unordered_map<std::string, RequestCb *>m_requestCBMap;
	std::mutex m_notifyMutex;
	std::multimap<std::string, NotifyCb*>m_NotifyCBMap;
	std::list<NotifyCb*>m_NotifyAllList;
	RequestItemCb* m_topicesRequestQueryCb = nullptr;
	ConsumedVarInfoCb* m_topicesVariableSubCb = nullptr;
	ProductedVarInfoCb* m_topicesVariablePubCb = nullptr;
	std::list<std::shared_ptr<UdmJsonRefMsg<bool>>> pubBoolRefList;
	std::list<std::shared_ptr<UdmJsonRefMsg<SEF_Int64>>> pubInt64RefList;
	std::list<std::shared_ptr<UdmJsonRefMsg<int>>> pubIntRefList;
	std::list<std::shared_ptr<UdmJsonRefMsg<float>>> pubFloatRefList;
	std::list<std::shared_ptr<UdmJsonRefMsg<double>>> pubDoubleRefList;
	std::list<std::shared_ptr<UdmJsonRefMsg<string>>> pubStringRefList;

	std::unordered_map<std::string, UdmMessage *>m_lockMap;

	std::unordered_map<string, bool*> subBoolMap;
	std::unordered_map <string, int*> subIntMap;
	std::unordered_map <string, SEF_Int64*> subInt64Map;
	std::unordered_map <string, float*> subFloatMap;
	std::unordered_map <string, double*> subDoubleMap;
	std::unordered_map <string, string*> subStringMap;

	std::unordered_map<std::string, RequestInfo> m_requestItemMap;

	SafeQueue<std::shared_ptr<UdmMessage>> m_commitQueue;
	std::thread* m_commitThd = nullptr;
	std::thread* m_thread = nullptr;
	std::thread* m_publishThread = nullptr;
	bool m_enable;
};

SEF::Udm::UdmBusiness::UdmBusiness()
{
	m_ctx = new BusContent();
	m_ctx->m_thread = new thread(&UdmBusiness::notifyThreadCycle, this);
	m_ctx->m_commitThd = new thread(&UdmBusiness::notifyThreadCycle, this);
	m_ctx->m_publishThread = new thread(&UdmBusiness::commitThreadCycle, this);
}

SEF::Udm::UdmBusiness::~UdmBusiness()
{
	m_ctx->m_enable = false;
	m_ctx->m_commitQueue.terminate();
	if (m_ctx->m_thread&& m_ctx->m_thread->joinable())
	{
		m_ctx->m_thread->join();
	}
	m_ctx->m_commitQueue.terminate();
	if (m_ctx->m_commitThd && m_ctx->m_commitThd->joinable())
	{
		m_ctx->m_commitThd->join();
	}
	if (m_ctx->m_publishThread && m_ctx->m_publishThread->joinable())
	{
		m_ctx->m_publishThread->join();
	}

	delete m_ctx;
}

void SEF::Udm::UdmBusiness::read()
{
	std::shared_ptr<UdmMessage> udmMessage;
	SafeQueue<std::shared_ptr<UdmMessage>> queue  = std::move(m_ctx->m_manualQueue);
	while (queue.tryPop(udmMessage) == true)
	{
		string mapKey;
		mapKey = GenrateMapKey(udmMessage->GetTopicName(), udmMessage->GetVariableName(), udmMessage->GetVariableUnit());
		switch (udmMessage->GetMsgType()) {
		case ValueType::VALUE_TYPE_BOOL: {
			auto it = m_ctx->subBoolMap.find(mapKey);
			if (it != m_ctx->subBoolMap.end()) 
			{
				bool* slot = it->second;
				if (stoi(udmMessage->GetValueString()) == 1) 
				{
					*slot = true;
				}
				else 
				{
					*slot = false;
				}
				LOGFMTT("(queue count %lu)%p read notify:[%s][%s]locktype %d\n", queue.size(), this, mapKey.c_str(), udmMessage->GetValueString(), udmMessage->GetLockStat());
			}
			break;
		}
		case ValueType::VALUE_TYPE_INT: {
			auto it = m_ctx->subIntMap.find(mapKey);
			if (it != m_ctx->subIntMap.end()) 
			{
				*m_ctx->subIntMap[mapKey] = stoi(udmMessage->GetValueString());
				LOGFMTT("(queue count %lu)%p read notify:[%s][%s]locktype %d\n", queue.size(), this, mapKey.c_str(), udmMessage->GetValueString(), udmMessage->GetLockStat());
			}
			break;
		}
		case ValueType::VALUE_TYPE_INT64: {
			auto it = m_ctx->subInt64Map.find(mapKey);
			if (it != m_ctx->subInt64Map.end())
			{
				*m_ctx->subInt64Map[mapKey] = stoll(udmMessage->GetValueString());
				LOGFMTT("(queue count %lu)%p read notify:[%s][%s]locktype %d\n", queue.size(), this, mapKey.c_str(), udmMessage->GetValueString(), udmMessage->GetLockStat());
			}
			break;
		}
		case ValueType::VALUE_TYPE_FLOAT: {
			auto it = m_ctx->subFloatMap.find(mapKey);
			if (it != m_ctx->subFloatMap.end()) 
			{
				*m_ctx->subFloatMap[mapKey] = stof(udmMessage->GetValueString());
				LOGFMTT("(queue count %lu)%p read notify:[%s][%s]locktype %d\n", queue.size(), this, mapKey.c_str(), udmMessage->GetValueString(), udmMessage->GetLockStat());
			}
			break;
		}
		case ValueType::VALUE_TYPE_DOUBLE: {
			auto it = m_ctx->subDoubleMap.find(mapKey);
			if (it != m_ctx->subDoubleMap.end()) 
			{
				*m_ctx->subDoubleMap[mapKey] = stod(udmMessage->GetValueString());
				LOGFMTT("(queue count %lu)%p read notify:[%s][%s]locktype %d\n", queue.size(), this, mapKey.c_str(), udmMessage->GetValueString(), udmMessage->GetLockStat());
			}
			break;
		}
		case ValueType::VALUE_TYPE_STRING: {
			auto it = m_ctx->subStringMap.find(mapKey);
			if (it != m_ctx->subStringMap.end()) 
			{
				string* slot = it->second;
				*slot = udmMessage->GetValueString();
				LOGFMTT("(queue count %lu)%p read notify:[%s][%s]locktype %d\n", queue.size(), this, mapKey.c_str(), udmMessage->GetValueString(), udmMessage->GetLockStat());
			}
			break;
		}
		case ValueType::VALUE_TYPE_REQUEST: {
			break;
		}
		default:
			LOGFMTE("pop msg success, but msg type error: key %s  type %d", mapKey.c_str(), udmMessage->GetMsgType());
			continue;
		}
	}
}

bool SEF::Udm::UdmBusiness::manualCommit(ValueType type, string& value, size_t valueSize,
	std::shared_ptr<UdmMsg> msg)
{
	bool ret = false;
	std::shared_ptr<UdmMessage> udmMessage = std::make_shared<UdmMessage>();
	string topicName = msg->GetTopicName();
	string variableName = msg->GetVariableName();
	string variableUnit = msg->GetVariableUnit();
	udmMessage->SetSourceName(msg->GetSourceName());
	udmMessage->SetTimestamp(msg->GetTimestamp());
	udmMessage->SetMsgType(type);
	udmMessage->SetValue(value);
	udmMessage->SetTopicName(topicName);
	udmMessage->SetVariableName(variableName);
	udmMessage->SetVariableUnit(variableUnit);

	if (udmMessage->serialize(type, value, valueSize, topicName, variableName, variableUnit))
	{
		ret = m_ctx->m_commitQueue.push(udmMessage);
	}
	LOGFMTT("m_commitQueue size %lu", m_ctx->m_commitQueue.size());
	return ret;
}

bool SEF::Udm::UdmBusiness::buildSubItem(ValueType nodeType, const string& topic, const string& variableName, const string& variableUnit, UdmMessage & udmMessage, INTERNAL::TreeItemType treeItemType)
{
	topicList list;
	udmMessage.SetMsgType(ValueType::VALUE_TYPE_TOPICNODE);
	ValueType type = ValueType::VALUE_TYPE_TOPICNODE;
	string msgTopic;
	Node* node = list.add_nodes();
	node->set_name(variableName+ SEF::INTERNAL::ITEM_SPLIC_CHAR + variableUnit);
	node->set_topicname(topic);
	node->set_type((int32_t)nodeType);
	list.set_type((int32_t)treeItemType);
	switch (treeItemType)
	{
	case INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_SUB: {
		msgTopic = TOPIC_QUERY_VAR_SUB_TOPICNAME;
		break;
	}
	case INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_PUB: {
		msgTopic = TOPIC_QUERY_VAR_PUB_TOPICNAME;
		break;
	}
	case INTERNAL::TreeItemType::TOPICQUERY_REQUEST: {
		msgTopic = TOPIC_QUERY_REQUEST_TOPICNAME;
		break;
	}
	default:
		LOGFMTT("build item failed: error type %d", treeItemType);
		return false;
	}

	SEF_Size_t payloadSize = list.ByteSizeLong();
	ByteArray array(payloadSize);
	SEF_Size_t size = 0;
	SEF_Uchar* ptr = array.getBytes(size);
	list.SerializeToArray(ptr, payloadSize);

	udmMessage.SetMsgType(type);
	udmMessage.SetTopicName(topic);
	udmMessage.SetVariableName(variableName);
	udmMessage.SetVariableUnit(variableUnit);
	udmMessage.SetValue(&array);
	if (udmMessage.serialize(array, type, msgTopic, string(), string()))
	{
		return true;
	}

	return false;
}
void SEF::Udm::UdmBusiness::commitThreadCycle() 
{
	while (m_ctx->m_enable == true) 
	{
		std::shared_ptr<UdmMessage> udmMessage;
		if (m_ctx->m_commitQueue.pop(udmMessage) == true) 
		{
			publish(udmMessage);
		}
	}
}

void SEF::Udm::UdmBusiness::notifyThreadCycle()
{
	while (m_ctx->m_enable == true) 
	{
		std::shared_ptr<UdmMessage> udmMessage;
		MessageInfo index;
		if (m_ctx->m_notifyQueue.pop(udmMessage) == true) 
		{
			index.topicName = udmMessage->GetTopicName();
			index.variableName = udmMessage->GetVariableName();
			index.variableUnit = udmMessage->GetVariableUnit();
			index.timestamp = udmMessage->GetTimestamp();
			index.sourceName = udmMessage->GetSourceName();
			index.type = udmMessage->GetMsgType();
			switch (udmMessage->GetMsgType()) {
			case ValueType::VALUE_TYPE_BOOL: {
				bool ret = false;
				if (stoi(udmMessage->GetValueString()) == 1) 
				{
					ret = true;
				}
				else 
				{
					ret = false;
				}
				NotifyVariable(index, ret);
				break;
			}
			case ValueType::VALUE_TYPE_INT: {
				int ret = stoi(udmMessage->GetValueString());
				NotifyVariable(index, ret);
				break;
			}
			case ValueType::VALUE_TYPE_INT64: {
				SEF_Int64 ret = stoll(udmMessage->GetValueString());
				NotifyVariable(index, ret);
				break;
			}
			case ValueType::VALUE_TYPE_FLOAT: {
				float ret = stof(udmMessage->GetValueString());
				NotifyVariable(index, ret);
				break;
			}
			case ValueType::VALUE_TYPE_DOUBLE: {
				double ret = stod(udmMessage->GetValueString());
				NotifyVariable(index, ret);
				break;
			}
			case ValueType::VALUE_TYPE_STRING: {
				NotifyVariable(index, udmMessage->GetValueString());
				break;
			}
			case ValueType::VALUE_TYPE_REQUEST: {
				std::unique_lock<std::mutex> lock(m_ctx->m_mapMutex);
				auto it = m_ctx->m_requestCBMap.find(udmMessage->GetTopicName());
				if (it != m_ctx->m_requestCBMap.end()) 
				{
					lock.unlock();
					RequestCb * requestCb = it->second;
					RequestInfo info;
					info.topicName = index.topicName;
					info.queueName = index.variableName;
					info.requestName = index.variableUnit;
					requestCb->OnRequestRecv(info, *udmMessage->GetValue());
				}
				break;
			}
			case ValueType::VALUE_TYPE_REQUESTQUEUE: {
				break;
			}
			case ValueType::VALUE_TYPE_TOPICNODE: {
				NotifyTopicItem(udmMessage);
				break;
			}
			default:
				LOGFMTT("pop msg success, but msg type error: %s", udmMessage->GetTopicName().c_str());
				break;
			}
		}
	}
}

void SEF::Udm::UdmBusiness::commit()
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
	}
	for (std::shared_ptr<UdmJsonRefMsg<bool>> msg : m_ctx->pubBoolRefList) 
	{
		bool output = msg->getRef();
		string value = std::to_string(output);
		msg->SetTimestamp(getCurrentSec());
		manualCommit(ValueType::VALUE_TYPE_BOOL, value, sizeof(bool), msg);
	}
	for (std::shared_ptr<UdmJsonRefMsg<int>> msg : m_ctx->pubIntRefList)
	{
		int output = msg->getRef();
		string value = std::to_string(output);
		msg->SetTimestamp(getCurrentSec());
		manualCommit(ValueType::VALUE_TYPE_INT, value, sizeof(int), msg);
	}
	for (std::shared_ptr<UdmJsonRefMsg<SEF_Int64>> msg : m_ctx->pubInt64RefList)
	{
		SEF_Int64 output = msg->getRef();
		string value = std::to_string(output);
		msg->SetTimestamp(getCurrentSec());
		manualCommit(ValueType::VALUE_TYPE_INT64, value, sizeof(int), msg);
	}
	for (std::shared_ptr<UdmJsonRefMsg<float>> msg : m_ctx->pubFloatRefList)
	{
		float output = msg->getRef();
		string value = std::to_string(output);
		msg->SetTimestamp(getCurrentSec());
		manualCommit(ValueType::VALUE_TYPE_FLOAT, value, sizeof(float), msg);
	}
	for (std::shared_ptr<UdmJsonRefMsg<double>> msg : m_ctx->pubDoubleRefList)
	{
		double output = msg->getRef();
		string value = std::to_string(output);
		msg->SetTimestamp(getCurrentSec());
		manualCommit(ValueType::VALUE_TYPE_DOUBLE, value, sizeof(double), msg);
	}
	for (std::shared_ptr<UdmJsonRefMsg<string>> msg : m_ctx->pubStringRefList)
	{
		string value = msg->getRef();
		msg->SetTimestamp(getCurrentSec());
		manualCommit(ValueType::VALUE_TYPE_STRING, value, value.size(), msg);
	}
}

bool SEF::Udm::UdmBusiness::publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::shared_ptr<UdmJsonRefMsg<bool>> json = std::make_shared<UdmJsonRefMsg<bool>>(output);

	json->SetMsgType(ValueType::VALUE_TYPE_BOOL);
	json->SetTopicName(topicName);
	json->SetVariableName(variableName);
	json->SetVariableUnit(variableUnit);
	json->SetSourceName(udm_name);
	m_ctx->pubBoolRefList.push_back(json);

	UdmMessage udmMessage;
	bool ret = buildSubItem(ValueType::VALUE_TYPE_BOOL, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_PUB);
	if (ret == true)
	{
		pushSubItem(udmMessage);
	}
	return true;
}

bool SEF::Udm::UdmBusiness::publish(SEF_Int64& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::shared_ptr<UdmJsonRefMsg<SEF_Int64>> json = std::make_shared<UdmJsonRefMsg<SEF_Int64>>(output);

	json->SetMsgType(ValueType::VALUE_TYPE_INT64);
	json->SetTopicName(topicName);
	json->SetVariableName(variableName);
	json->SetVariableUnit(variableUnit);
	json->SetSourceName(udm_name);
	m_ctx->pubInt64RefList.push_back(json);

	UdmMessage udmMessage;
	bool ret = buildSubItem(ValueType::VALUE_TYPE_INT64, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_PUB);
	if (ret == true)
	{
		pushSubItem(udmMessage);
	}
	return true;
}

bool SEF::Udm::UdmBusiness::publish(int& output, const SEF_String& topicName, const SEF_String& variableName, const string& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::shared_ptr<UdmJsonRefMsg<int>> json = std::make_shared<UdmJsonRefMsg<int>>(output);

	json->SetMsgType(ValueType::VALUE_TYPE_INT);
	json->SetTopicName(topicName);
	json->SetVariableName(variableName);
	json->SetVariableUnit(variableUnit);
	json->SetSourceName(udm_name);
	m_ctx->pubIntRefList.push_back(json);

	UdmMessage udmMessage;
	bool ret = buildSubItem(ValueType::VALUE_TYPE_INT,topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_PUB);
	if (ret == true)
	{
		pushSubItem(udmMessage);
	}
	return true;
}

bool SEF::Udm::UdmBusiness::publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const string& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::shared_ptr<UdmJsonRefMsg<float>> json = std::make_shared<UdmJsonRefMsg<float>>(output);

	json->SetMsgType(ValueType::VALUE_TYPE_FLOAT);
	json->SetTopicName(topicName);
	json->SetVariableName(variableName);
	json->SetVariableUnit(variableUnit);
	json->SetSourceName(udm_name);
	m_ctx->pubFloatRefList.push_back(json);

	UdmMessage udmMessage;
	bool ret = buildSubItem(ValueType::VALUE_TYPE_FLOAT, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_PUB);
	if (ret == true)
	{
		pushSubItem(udmMessage);
	}
	return true;
}

bool SEF::Udm::UdmBusiness::publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const string& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::shared_ptr<UdmJsonRefMsg<double>> json = std::make_shared<UdmJsonRefMsg<double>>(output);

	json->SetMsgType(ValueType::VALUE_TYPE_DOUBLE);
	json->SetTopicName(topicName);
	json->SetVariableName(variableName);
	json->SetVariableUnit(variableUnit);
	json->SetSourceName(udm_name);
	m_ctx->pubDoubleRefList.push_back(json);

	UdmMessage udmMessage;
	bool ret = buildSubItem(ValueType::VALUE_TYPE_DOUBLE, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_PUB);
	if (ret == true)
	{
		pushSubItem(udmMessage);
	}
	return true;
}

bool SEF::Udm::UdmBusiness::publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::shared_ptr<UdmJsonRefMsg<string>> json = std::make_shared<UdmJsonRefMsg<string>>(output);

	json->SetMsgType(ValueType::VALUE_TYPE_STRING);
	json->SetTopicName(topicName);
	json->SetVariableName(variableName);
	json->SetVariableUnit(variableUnit);
	json->SetSourceName(udm_name);
	m_ctx->pubStringRefList.push_back(json);

	UdmMessage udmMessage;
	bool ret = buildSubItem(ValueType::VALUE_TYPE_STRING, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_PUB);
	if (ret == true)
	{
		pushSubItem(udmMessage);
	}
	return true;
}

bool SEF::Udm::UdmBusiness::subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	string key = GenrateMapKey(topicName, variableName, variableUnit);
	auto it = m_ctx->subBoolMap.find(key);
	if (it == m_ctx->subBoolMap.end()) 
	{
		subscribe(topicName);
		UdmMessage udmMessage;
		bool ret = buildSubItem(ValueType::VALUE_TYPE_BOOL, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_SUB);
		if (ret == true) 
		{
			pushSubItem(udmMessage);
		}
		m_ctx->subBoolMap.insert(std::make_pair(key, &input));
		return true;
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribe(SEF_Int64& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	string key = GenrateMapKey(topicName, variableName, variableUnit);
	auto it = m_ctx->subInt64Map.find(key);
	if (it == m_ctx->subInt64Map.end())
	{
		subscribe(topicName);
		UdmMessage udmMessage;
		bool ret = buildSubItem(ValueType::VALUE_TYPE_INT64, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_SUB);
		if (ret == true)
		{
			pushSubItem(udmMessage);
		}
		m_ctx->subInt64Map.insert(std::make_pair(key, &input));
		return true;
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const string& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	string key = GenrateMapKey(topicName, variableName, variableUnit);
	auto it = m_ctx->subIntMap.find(key);
	if (it == m_ctx->subIntMap.end()) 
	{
		subscribe(topicName);
		UdmMessage udmMessage;
		bool ret = buildSubItem(ValueType::VALUE_TYPE_INT,topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_SUB);
		if (ret == true) 
		{
			pushSubItem(udmMessage);
		}
		m_ctx->subIntMap.insert(std::make_pair(key, &input));
		return true;
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const string& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	string key = GenrateMapKey(topicName, variableName, variableUnit);
	auto it = m_ctx->subFloatMap.find(key);
	if (it == m_ctx->subFloatMap.end()) 
	{
		subscribe(topicName);
		UdmMessage udmMessage;
		bool ret = buildSubItem(ValueType::VALUE_TYPE_FLOAT, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_SUB);
		if (ret == true) 
		{
			pushSubItem(udmMessage);
		}
		m_ctx->subFloatMap.insert(std::make_pair(key, &input));
		return true;
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const string& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	string key = GenrateMapKey(topicName, variableName, variableUnit);
	auto it = m_ctx->subDoubleMap.find(key);
	if (it == m_ctx->subDoubleMap.end()) 
	{
		subscribe(topicName);
		UdmMessage udmMessage;
		bool ret = buildSubItem(ValueType::VALUE_TYPE_DOUBLE, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_SUB);
		if (ret == true) 
		{
			pushSubItem(udmMessage);
		}
		m_ctx->subDoubleMap.insert(std::make_pair(key, &input));
		return true;
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	string key = GenrateMapKey(topicName, variableName, variableUnit);
	auto it = m_ctx->subStringMap.find(key);
	if (it == m_ctx->subStringMap.end()) 
	{
		subscribe(topicName);
		UdmMessage udmMessage;
		bool ret = buildSubItem(ValueType::VALUE_TYPE_STRING, topicName, variableName, variableUnit, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_VARIABLE_SUB);
		if (ret == true) 
		{
			pushSubItem(udmMessage);
		}
		m_ctx->subStringMap.insert(std::make_pair(key, &input));
		return true;
	}
	return false;
}

bool SEF::Udm::UdmBusiness::lock(const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit, std::any v)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	SEF_String id = topicName + variableName;
	UdmMessage* udmMessage = nullptr;

	auto it = m_ctx->m_lockMap.find(id);
	if (it == m_ctx->m_lockMap.end())
	{
		udmMessage = new UdmMessage();
		udmMessage->SetLockStat(LockStat::LOCK);
		udmMessage->SetTimestamp(getCurrentSec());
		udmMessage->SetTopicName(topicName);
		udmMessage->SetVariableName(variableName);
		udmMessage->SetVariableUnit(variableUnit);
		udmMessage->SetSourceName(udm_name);
		if (v.type() == typeid(bool))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_BOOL);
			udmMessage->SetValue(std::to_string(std::any_cast<bool>(v)));
		}
		else if (v.type() == typeid(int))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_INT);
			udmMessage->SetValue(std::to_string(std::any_cast<int>(v)));
		}
		else if (v.type() == typeid(float))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_FLOAT);
			udmMessage->SetValue(std::to_string(std::any_cast<float>(v)));
		}
		else if (v.type() == typeid(double))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_DOUBLE);
			udmMessage->SetValue(std::to_string(std::any_cast<double>(v)));
		}
		else if (v.type() == typeid(std::string))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_STRING);
			udmMessage->SetValue(std::any_cast<std::string>(v));
		}
		else if (v.type() == typeid(int64_t))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_INT64);
			int64_t v64 = std::any_cast<int64_t>(v);
			udmMessage->SetValue(std::to_string(v64));
		}
		else
		{
			delete udmMessage;
			LOGFMTD("lock failed, unsupport any type %s", v.type().name());
			return false;
		}
		m_ctx->m_lockMap.insert(std::make_pair(id, udmMessage));
	}
	else
	{
		udmMessage = it->second;
		if (v.type() == typeid(bool))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_BOOL);
			udmMessage->SetValue(std::to_string(std::any_cast<bool>(v)));
		}
		else if (v.type() == typeid(int))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_INT);
			udmMessage->SetValue(std::to_string(std::any_cast<int>(v)));
		}
		else if (v.type() == typeid(float))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_FLOAT);
			udmMessage->SetValue(std::to_string(std::any_cast<float>(v)));
		}
		else if (v.type() == typeid(double))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_DOUBLE);
			udmMessage->SetValue(std::to_string(std::any_cast<double>(v)));
		}
		else if (v.type() == typeid(int64_t))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_INT64);
			int64_t v64 = std::any_cast<int64_t>(v);
			udmMessage->SetValue(std::to_string(v64));
		}
		else if (v.type() == typeid(std::string))
		{
			udmMessage->SetMsgType(ValueType::VALUE_TYPE_STRING);
			udmMessage->SetValue(std::any_cast<std::string>(v));
		}
		else
		{
			LOGFMTD("lock failed, unsupport any type %s", v.type().name());
			return false;
		}
	}

	std::shared_ptr<UdmMessage> udmPushMessage = std::make_shared<UdmMessage>();
	udmPushMessage->SetLockStat(udmMessage->GetLockStat());
	udmPushMessage->SetTimestamp(udmMessage->GetTimestamp());
	udmPushMessage->SetTopicName(udmMessage->GetTopicName());
	udmPushMessage->SetVariableName(udmMessage->GetVariableName());
	udmPushMessage->SetVariableUnit(udmMessage->GetVariableUnit());
	udmPushMessage->SetSourceName(udmMessage->GetSourceName());
	udmPushMessage->SetMsgType(udmMessage->GetMsgType());
	udmPushMessage->SetValue(udmMessage->GetValue());
	if (SEF_FALSE == udmPushMessage->serialize(udmPushMessage->GetMsgType(), const_cast<SEF_String&>(udmMessage->GetValueString()), sizeof(int), const_cast<SEF_String&>(topicName), const_cast<SEF_String&>(variableName), const_cast<SEF_String&>(variableUnit)))
	{
		return false;
	}
	return m_ctx->m_commitQueue.push(udmPushMessage);
}

bool SEF::Udm::UdmBusiness::unlock(const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	SEF_String id = topicName + variableName;
	auto it = m_ctx->m_lockMap.find(id);
	if (it != m_ctx->m_lockMap.end())
	{
		UdmMessage *udmMessage = it->second;
		udmMessage->SetLockStat(LockStat::UNLOCK);

		std::shared_ptr<UdmMessage> udmPushMessage = std::make_shared<UdmMessage>();
		udmPushMessage->SetLockStat(udmMessage->GetLockStat());
		udmPushMessage->SetTimestamp(udmMessage->GetTimestamp());
		udmPushMessage->SetTopicName(udmMessage->GetTopicName());
		udmPushMessage->SetVariableName(udmMessage->GetVariableName());
		udmPushMessage->SetVariableUnit(udmMessage->GetVariableUnit());
		udmPushMessage->SetSourceName(udmMessage->GetSourceName());
		udmPushMessage->SetMsgType(udmMessage->GetMsgType());
		udmPushMessage->SetValue(udmMessage->GetValue());
		if (SEF_FALSE == udmPushMessage->serialize())
		{
			return false;
		}

		m_ctx->m_commitQueue.push(udmPushMessage);
		m_ctx->m_lockMap.erase(it);
		delete udmMessage;
	}
	return true;
}

void SEF::Udm::UdmBusiness::disableRead()
{
	m_ctx->m_manualQueueEnable = false;
}

bool SEF::Udm::UdmBusiness::sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::shared_ptr<UdmMessage> udmMessage = std::make_shared<UdmMessage>();
	udmMessage->SetMsgType(ValueType::VALUE_TYPE_REQUEST);
	ValueType type = ValueType::VALUE_TYPE_REQUEST;
	string variableName = info.queueName;
	string variableUint = info.requestName;
	udmMessage->SetSourceName(udm_name);
	udmMessage->SetTimestamp(getCurrentSec());
	udmMessage->SetTopicName(info.topicName);
	udmMessage->SetVariableName(info.queueName);

	if (udmMessage->serialize(bytes, type, info.topicName, variableName, variableUint))
	{
		return m_ctx->m_commitQueue.push(udmMessage);
	}
	return false;
}


void SEF::Udm::UdmBusiness::onRecv(std::shared_ptr<UdmMessage> message)
{
	if (m_ctx->m_manualQueueEnable == true)
	{
		m_ctx->m_manualQueue.push(message);
	}
	m_ctx->m_notifyQueue.push(message);
}

bool SEF::Udm::UdmBusiness::subscribe(NotifyCb* cb, const VarQueryInfo& info)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	if (info.topicName.empty() == false) 
	{
		SEF_String key = info.topicName + info.varName;

		subscribe(info.topicName);
		std::lock_guard<std::mutex> lock(m_ctx->m_notifyMutex);
		m_ctx->m_NotifyCBMap.insert(std::make_pair(key, cb));
		return true;
	}
	else {
		subscribe(info.topicName);
		m_ctx->m_NotifyAllList.push_back(cb);
	}

	return false;
}

bool SEF::Udm::UdmBusiness::recvRequest(const RequestQueryInfo& info, RequestCb * cb)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected ly");
		return false;
	}
	bool ret = subscribe(info.topicName);
	std::lock_guard<std::mutex> lock(m_ctx->m_mapMutex);
	auto it = m_ctx->m_requestCBMap.find(info.topicName);
	if(it!= m_ctx->m_requestCBMap.end())
	{
		return false;
	}
	m_ctx->m_requestCBMap.insert(std::make_pair(info.topicName, cb));
	return ret;
}


bool SEF::Udm::UdmBusiness::subscribeRequestQuery(RequestItemCb* cb)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	m_ctx->m_topicesRequestQueryCb = cb;
	subscribeRequestItemDir(TreeItemType::TOPICQUERY_REQUEST, TOPIC_QUERY_REQUEST_TOPICNAME,cb);
	return true;
}

bool SEF::Udm::UdmBusiness::subscribeConsumedVarInfo(ConsumedVarInfoCb* cb)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	m_ctx->m_topicesVariableSubCb = cb;
	subscribeItemDir(TreeItemType::TOPICQUERY_VARIABLE_SUB, TOPIC_QUERY_VAR_SUB_TOPICNAME,cb);
	return true;
}

bool SEF::Udm::UdmBusiness::subscribeProductedVarInfo(ProductedVarInfoCb* cb)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	m_ctx->m_topicesVariablePubCb = cb;
	subscribeItemDir(TreeItemType::TOPICQUERY_VARIABLE_PUB, TOPIC_QUERY_VAR_PUB_TOPICNAME, cb);
	return true;
}


bool SEF::Udm::UdmBusiness::unsubscribeNotify(const VarQueryInfo& info, NotifyCb* cb)
{
	if (info.topicName.empty() == false)
	{
		std::lock_guard<std::mutex> lock(m_ctx->m_notifyMutex);
		SEF_String key = info.topicName + info.varName;

		auto range = m_ctx->m_NotifyCBMap.equal_range(key);
		for (auto it = range.first;it!=range.second;++it)
		{
			if (it->second == cb)
			{
				m_ctx->m_NotifyCBMap.erase(it);
			}
		}
	}
	return false;
}

bool SEF::Udm::UdmBusiness::unsubscribeNotify(NotifyCb* cb)
{
	std::lock_guard<std::mutex> lock(m_ctx->m_notifyMutex);
	for (auto it = m_ctx->m_NotifyCBMap.begin(); it !=m_ctx->m_NotifyCBMap.end();)
	{
		if (it->second == cb)
		{
			it = m_ctx->m_NotifyCBMap.erase(it);
		}
		else
		{
			++it;
		}
	}
	return true;
}

bool SEF::Udm::UdmBusiness::unsubscribeRequest(const RequestQueryInfo& info)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	std::lock_guard<std::mutex> lock(m_ctx->m_mapMutex);
	if (info.topicName.empty() == false)
	{
		auto it = m_ctx->m_requestCBMap.find(info.topicName);
		if (it != m_ctx->m_requestCBMap.end())
		{
			m_ctx->m_requestCBMap.erase(it);
			return true;
		}
		unsubscribe(info.topicName);
	}
	return false;
}

bool SEF::Udm::UdmBusiness::registerRequest(const RequestQueryInfo& info)
{
	if (isUdmConnected() == false)
	{
		LOGFMTD("check if udm is connected successfully");
		return false;
	}
	if (info.queueName.empty() == false && info.requestName.empty() == false)
	{
		UdmMessage udmMessage;
		SEF_String key = GenrateMapKey(info.topicName, info.queueName, info.requestName);
		if (m_ctx->m_requestItemMap.count(key) == false)
		{
			bool ret = buildSubItem(ValueType::VALUE_TYPE_REQUEST, info.topicName, info.queueName, info.requestName, udmMessage, INTERNAL::TreeItemType::TOPICQUERY_REQUEST);
			if (ret == true)
			{
				pushSubItem(udmMessage);
			}
			RequestInfo requestInfo;
			requestInfo.topicName = info.topicName;
			requestInfo.queueName = info.queueName;
			requestInfo.requestName = info.requestName;
			m_ctx->m_requestItemMap.insert(make_pair(key, requestInfo));
			return true;
		}
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribeItemDir(TreeItemType toicpQueryType, const string &topic, ConsumedVarInfoCb* cb)
{
	UdmMessage udmMessage;
	ValueType type = ValueType::VALUE_TYPE_QUERY_TOPICNODE;
	udmMessage.SetMsgType(type);

	topicList list;
	int rc = -1;
	list.set_type((int32_t)toicpQueryType);

	subscribe(topic);

	SEF_Size_t payloadSize = list.ByteSizeLong();
	ByteArray array(payloadSize);
	SEF_Size_t size = 0;
	SEF_Uchar* ptr = array.getBytes(size);
	list.SerializeToArray(ptr, payloadSize);

	if (udmMessage.serialize(array, type, topic, string(), string()))
	{
		UdmMessage response;
		bool result = _sendRequest(udmMessage, response);
		if (result == true) 
		{
			const ByteArray* array = response.GetValue();
			topicList list;
			SEF_Size_t size = 0;
			void* data = array->getBytes(size);
			if (size > 0)
			{
				if (list.ParseFromArray(data, size) == false)
				{
					LOGFMTD("request item failed, parse result error");
					return false;
				}
				std::vector<MessageInfo> vec;
				int node_size = list.nodes_size();
				for (int i = 0; i < node_size; i++)
				{
					const Node& node = list.nodes(i);
					MessageInfo info;
					info.topicName = node.topicname();					
					SEF_String param = node.name();
					size_t start = 0;
					size_t end = param.find(SEF::INTERNAL::ITEM_SPLIC_CHAR);
					info.variableName = param.substr(start, end);
					info.variableUnit = param.substr(end+1, param.size());
					info.type = static_cast<ValueType>(node.type());
					vec.push_back(info);
					LOGFMTT("udm recv items type %d,add %s - %s", toicpQueryType, info.topicName.c_str(), info.variableName.c_str());
				}
				cb->OnConsumedVarInfoRecv(vec);
			}
			return true;
		}
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribeItemDir(INTERNAL::TreeItemType toicpQueryType, const string& topic, ProductedVarInfoCb* cb)
{
	UdmMessage udmMessage;
	ValueType type = ValueType::VALUE_TYPE_QUERY_TOPICNODE;
	udmMessage.SetMsgType(type);

	topicList list;
	int rc = -1;
	list.set_type((int32_t)toicpQueryType);

	subscribe(topic);

	SEF_Size_t payloadSize = list.ByteSizeLong();
	ByteArray array(payloadSize);
	SEF_Size_t size = 0;
	SEF_Uchar* ptr = array.getBytes(size);
	list.SerializeToArray(ptr, payloadSize);

	if (udmMessage.serialize(array, type, topic, string(), string()))
	{
		UdmMessage response;
		bool result = _sendRequest(udmMessage, response);
		if (result == true)
		{
			const ByteArray* array = response.GetValue();
			topicList list;
			SEF_Size_t size = 0;
			void* data = array->getBytes(size);
			if (size > 0)
			{
				if (list.ParseFromArray(data, size) == false)
				{
					LOGFMTD("request item failed, parse result error");
					return false;
				}
				std::vector<MessageInfo> vec;
				int node_size = list.nodes_size();
				for (int i = 0; i < node_size; i++)
				{
					const Node& node = list.nodes(i);
					MessageInfo info;
					info.topicName = node.topicname();
					SEF_String param = node.name();
					size_t start = 0;
					size_t end = param.find(SEF::INTERNAL::ITEM_SPLIC_CHAR);
					info.variableName = param.substr(start, end);
					info.variableUnit = param.substr(end + 1, param.size());
					info.type = static_cast<ValueType>(node.type());
					vec.push_back(info);
					LOGFMTT("udm recv items type %d,add %s - %s", toicpQueryType, info.topicName.c_str(), info.variableName.c_str());
				}
				cb->OnProductedVarInfoRecv(vec);
			}
			return true;
		}
	}
	return false;
}

bool SEF::Udm::UdmBusiness::subscribeRequestItemDir(TreeItemType toicpQueryType, const string& topic, RequestItemCb* cb)
{
	UdmMessage udmMessage;
	ValueType type = ValueType::VALUE_TYPE_QUERY_TOPICNODE;
	udmMessage.SetMsgType(type);

	topicList list;
	int rc = -1;
	list.set_type((int32_t)toicpQueryType);

	subscribe(topic);

	SEF_Size_t payloadSize = list.ByteSizeLong();
	ByteArray array(payloadSize);
	SEF_Size_t size = 0;
	SEF_Uchar* ptr = array.getBytes(size);
	list.SerializeToArray(ptr, payloadSize);

	if (udmMessage.serialize(array, type, topic, string(), string()))
	{
		UdmMessage response;
		bool result = _sendRequest(udmMessage, response);
		if (result == true)
		{
			const ByteArray* array = response.GetValue();
			topicList list;
			SEF_Size_t size = 0;
			void* data = array->getBytes(size);
			if (size > 0)
			{
				if (list.ParseFromArray(data, size) == false)
				{
					LOGFMTD("request item failed, parse result error");
					return false;
				}
				std::vector<RequestInfo> vec;
				int node_size = list.nodes_size();
				for (int i = 0; i < node_size; i++)
				{
					const Node& node = list.nodes(i);
					RequestInfo info;
					info.topicName = node.topicname();
					SEF_String param = node.name();
					size_t start = 0;
					size_t end = param.find(SEF::INTERNAL::ITEM_SPLIC_CHAR);
					info.queueName = param.substr(start, end);
					info.requestName = param.substr(end + 1, param.size());
					vec.push_back(info);
					LOGFMTT("udm recv items type %d,add %s - %s", toicpQueryType, info.topicName.c_str(), info.queueName.c_str());
				}
				cb->OnRequestItemRecv(vec);
			}
			return true;
		}
	}
	return false;
}

void SEF::Udm::UdmBusiness::NotifyVariable(const MessageInfo& index, std::any value)
{
	std::lock_guard<std::mutex> lock(m_ctx->m_notifyMutex);
	SEF_String key = index.topicName + index.variableName;
	auto it = m_ctx->m_NotifyCBMap.find(key);
	if (it != m_ctx->m_NotifyCBMap.end())
	{
		auto range = m_ctx->m_NotifyCBMap.equal_range(key);
		for (auto rangeIt = range.first; rangeIt != range.second; ++rangeIt)
		{
			NotifyCb* cb = rangeIt->second;
			cb->OnVarRecv(index, value);
		}
	}

	it = m_ctx->m_NotifyCBMap.find(index.topicName);
	if (it != m_ctx->m_NotifyCBMap.end())
	{
		auto range = m_ctx->m_NotifyCBMap.equal_range(index.topicName);
		for (auto rangeIt = range.first; rangeIt != range.second; ++rangeIt)
		{
			NotifyCb* cb = rangeIt->second;
			cb->OnVarRecv(index, value);
		}
	}

	for (NotifyCb* cb : m_ctx->m_NotifyAllList)
	{
		cb->OnVarRecv(index, value);
	}
}

void SEF::Udm::UdmBusiness::NotifyTopicItem(std::shared_ptr<UdmMessage> msg)
{
	topicList list;
	SEF_Size_t size = 0;
	const ByteArray* array = msg->GetValue();
	SEF_Uchar* buf = array->getBytes(size);
	bool ret = list.ParseFromArray(buf, size);
	if (ret == false) 
	{
		LOGFMTE("parser topic Items false, buffer size %lu", size);
		return;
	}

	switch ((TreeItemType)list.type())
	{
		case TreeItemType::TOPICQUERY_REQUEST: {
			if (m_ctx->m_topicesRequestQueryCb != nullptr) 
			{
				std::vector<RequestInfo>vec;
				buildReqItemFromMsg(vec, list);
				m_ctx->m_topicesRequestQueryCb->OnRequestItemRecv(vec);
			}
			break;
		}
		case TreeItemType::TOPICQUERY_VARIABLE_SUB: {
			if (m_ctx->m_topicesVariableSubCb != nullptr) 
			{
				std::vector<MessageInfo>vec;
				buildVarItemFromMsg(vec, list);
				m_ctx->m_topicesVariableSubCb->OnConsumedVarInfoRecv(vec);
			}
			break;
		}
		case TreeItemType::TOPICQUERY_VARIABLE_PUB: {
			if (m_ctx->m_topicesVariablePubCb != nullptr)
			{
				std::vector<MessageInfo>vec;
				buildVarItemFromMsg(vec, list);
				m_ctx->m_topicesVariablePubCb->OnProductedVarInfoRecv(vec);
			}
			break;
		}
		default:
			break;
	}
}


void SEF::Udm::UdmBusiness::NotifyRequest(UdmMessage& msg)
{
}
