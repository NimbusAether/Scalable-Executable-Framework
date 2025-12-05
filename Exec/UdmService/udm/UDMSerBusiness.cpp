#include "UDMSerBusiness.h"
#include "UdmMessage.h"
#include "ByteArray.h"
#include "log.h"
#include "topicList.pb.h"
#include "internal.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif

using namespace SEF::UdmService;
using namespace SEF::Udm;
using namespace std;
using namespace UDM;
using namespace SEF::COMMON;
using namespace SEF::INTERNAL;
using SEF::Udm::UDMSerBusiness;
using  SEF::COMMON::SafeQueue;

UDMSerBusiness * UDMSerBusiness::instance = nullptr;

SEF_String getLockId(std::shared_ptr<UdmMessage> message)
{
	return message->GetTopicName() + message->GetVariableName();
}
SEF_String getItemKey(const SEF_String &topic, const SEF_String& variableName)
{
	return topic + "@" + variableName;
}

/**************************************************/
UDMSerBusiness* UDMSerBusiness::getInstance()
{
	if (instance == nullptr) {
		instance = new UDMSerBusiness();
	}
	return instance;
}

void UDMSerBusiness::freeInstance()
{
	if (instance) {
		delete instance;
		instance = nullptr;
	}
}

void UDMSerBusiness::addNotiify(const MQNotifier* nofitier)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_notifierList.push_back(nofitier);
}

bool UDMSerBusiness::flushItemLockedStatus(std::shared_ptr<UdmMessage> message, SEF_Bool locked)
{
	SEF_String key = getItemKey(message->GetTopicName(), message->GetVariableName() + SEF::INTERNAL::ITEM_SPLIC_CHAR + message->GetVariableUnit());

	if (message->GetMsgType() == ValueType::VALUE_TYPE_REQUEST)
	{
		auto it = m_RequestItems.find(key);
		if (it != m_RequestItems.end())
		{
			std::shared_ptr<UDM::Node> node = it->second;
			node->set_locked(locked);
		}
	}
	else
	{
		auto it = m_VariableSubItemMap.find(key);
		if (it != m_VariableSubItemMap.end())
		{
			std::shared_ptr<UDM::Node> node = it->second;
			node->set_locked(locked);
		}
		it = m_VariablePubItemMap.find(key);
		if (it != m_VariablePubItemMap.end())
		{
			std::shared_ptr<UDM::Node> node = it->second;
			node->set_locked(locked);
		}
	}
	return true;
}
bool UDMSerBusiness::onRecv(std::shared_ptr<UdmMessage> message)
{
	SEF_String id = getLockId(message);

	if (message->GetLockStat() == LockStat::NONE)
	{
		auto it = m_LockMsgMap.find(id);
		if (it != m_LockMsgMap.end())
		{
			LOGFMTT("drop message %s, variable locked", id.c_str());
			return false;
		}
	}
	else if (message->GetLockStat() == LockStat::UNLOCK)
	{
		auto it = m_LockMsgMap.find(id);
		if (it != m_LockMsgMap.end())
		{
			flushItemLockedStatus(message, false);
			m_LockMsgMap.erase(it);
			LOGFMTT("unlock %s, variable unlocking", id.c_str());
		}
		return true;
	}
	else if (message->GetLockStat() == LockStat::LOCK)
	{
		auto it = m_LockMsgMap.find(id);
		if (it == m_LockMsgMap.end())
		{
			flushItemLockedStatus(message, true);
			m_LockMsgMap.insert(std::make_pair(id, message));
			LOGFMTT("lock %s, variable locking", id.c_str());
		}
	}
	return m_allQueue.push(message);
}

bool UDMSerBusiness::onRequestAllItems(UdmMessage& request, UdmMessage& response)
{
	topicList list;
	SEF_Size_t size = 0;

	const ByteArray* array = request.GetValue();
	const SEF_Uchar* data = array->getBytes(size);
	if (list.ParseFromArray(data, size) == true) 
	{
		map<string, std::shared_ptr<Node>>* nodeMap = nullptr;
		if ((int)TreeItemType::TOPICQUERY_REQUEST == list.type()) 
		{
			nodeMap = &m_RequestItems;
		}
		else if ((int)TreeItemType::TOPICQUERY_VARIABLE_SUB == list.type())
		{
			nodeMap = &m_VariableSubItemMap;
		}
		else 
		{
			nodeMap = &m_VariablePubItemMap;
		}
		std::unique_lock<std::mutex> lock(m_itemsMutex);
		for (auto& [key, value] : *nodeMap) 
		{
			Node* tNode = list.add_nodes();
			tNode->set_name(value->name());
			tNode->set_topicname(value->topicname());
			tNode->set_type(value->type());
			tNode->set_locked(value->locked());
			LOGFMTT("items  type %d,add %s - %s", list.type(), value->topicname().c_str(), value->name().c_str());
		}
		lock.unlock();
		LOGFMTT("response item size %d", list.nodes_size());
		size = list.ByteSizeLong();
		ByteArray listArray(size);
		list.SerializeToArray(listArray.getBytes(), size);
		ValueType valueType = ValueType::VALUE_TYPE_TOPICNODE;
		if (response.serialize(listArray, valueType, string(), string(), string()))
		{
			return true;
		}
	}
	return false;
}


bool UDMSerBusiness::onRequestAllItems(UdmMessage& request, int step, std::list<UdmMessage>& list)
{
	topicList tTopicList;
	SEF_Size_t size = 0;
	int index = 0;

	const ByteArray* array = request.GetValue();
	const SEF_Uchar* data = array->getBytes(size);
	ValueType valueType = ValueType::VALUE_TYPE_TOPICNODE;

	if (tTopicList.ParseFromArray(data, size) == true)
	{
		map<string, std::shared_ptr<Node>>* nodeMap = nullptr;
		if ((int)TreeItemType::TOPICQUERY_REQUEST == tTopicList.type())
		{
			nodeMap = &m_RequestItems;
		}
		else if ((int)TreeItemType::TOPICQUERY_VARIABLE_SUB == tTopicList.type())
		{
			nodeMap = &m_VariableSubItemMap;
		}
		else
		{
			nodeMap = &m_VariablePubItemMap;
		}
		std::unique_lock<std::mutex> lock(m_itemsMutex);
		for (auto& [key, value] : *nodeMap)
		{
			if (index < step)
			{
				Node* tNode = tTopicList.add_nodes();
				tNode->set_name(value->name());
				tNode->set_topicname(value->topicname());
				tNode->set_type(value->type());
				tNode->set_locked(value->locked());
			}
			else
			{
				UdmMessage response;
				size = tTopicList.ByteSizeLong();
				ByteArray listArray(size);
				tTopicList.SerializeToArray(listArray.getBytes(), size);
				if (response.serialize(listArray, valueType, string(), string(), string()))
				{
					list.push_back(response);
				}
			}
		}
		lock.unlock();
		LOGFMTT("response list of items  size %d", list.size());
	}
	return true;
}

UDMSerBusiness::UDMSerBusiness():m_running(true)
{
	pPublishThd = std::thread(&UDMSerBusiness::publishThreadCycle, this);
	pItemThd = std::thread(&UDMSerBusiness::ItemThreadCycle, this);

}

UDMSerBusiness::~UDMSerBusiness()
{
	m_running = false;
	m_allQueue.terminate();
	if (pPublishThd.joinable())
	{
		pPublishThd.join();
	}
	std::lock_guard<std::mutex> lockNew(m_itemsNewMutex);
	m_itemsNewCond.notify_one();
	if (pItemThd.joinable())
	{
		pItemThd.join();
	}
}

void UDMSerBusiness::publishThreadCycle()
{
	while (m_running == true)
	{
		std::shared_ptr<UdmMessage> udmMessage;
		if (m_allQueue.pop(udmMessage)) 
		{
			ValueType type = udmMessage->GetMsgType();			
			for (const MQNotifier* notifer : m_notifierList)
			{
				notifer->onCommonMessage(udmMessage);
			}

			if (udmMessage->GetMsgType() == ValueType::VALUE_TYPE_TOPICNODE) 
			{
				topicList list;
				SEF_Size_t size = 0;
				const ByteArray* array = udmMessage->GetValue();
				const SEF_Uchar* data = array->getBytes(size);
				if (list.ParseFromArray(data, size) == true) 
				{
					int node_size = list.nodes_size();
					map<string, std::shared_ptr<Node>>* nodeMap = nullptr;
					SafeQueue<std::shared_ptr<Node>>* newQueue = nullptr;
					if ((int)TreeItemType::TOPICQUERY_REQUEST == list.type()) 
					{
						nodeMap = &m_RequestItems;
						newQueue = &m_NewRequestItems;
					}
					else if ((int)TreeItemType::TOPICQUERY_VARIABLE_SUB == list.type())
					{
						nodeMap = &m_VariableSubItemMap;
						newQueue = &m_NewVariableSubItemQueue;
					}
					else
					{
						nodeMap = &m_VariablePubItemMap;
						newQueue = &m_NewVariablePubItemQueue;
					}
					std::unique_lock<std::mutex> lock(m_itemsMutex);
					for (int i = 0; i < node_size; i++) 
					{
						std::shared_ptr<Node >node = std::make_shared<Node>(list.nodes(i));
						SEF_String key = getItemKey(node->topicname(), node->name());
						auto it = nodeMap->find(key);
						if (it == nodeMap->end()) {
							nodeMap->insert(std::make_pair(key, node));
							//newQueue->push(node);
							LOGFMTD("service bussiness recv item: %s %s[%lu]", node->topicname().c_str(), node->name().c_str(), newQueue->size());
						}
					}
					std::lock_guard<std::mutex> lockNew(m_itemsNewMutex);
					m_itemsNewCond.notify_one();
				}
			}
		}
	}
}

void SEF::Udm::UDMSerBusiness::ItemThreadCycle()
{
	while (m_running == true)
	{
		std::unique_lock<std::mutex> lock(m_itemsNewMutex);
		NewItemPops();
		m_itemsNewCond.wait(lock);
	}
}

bool SEF::Udm::UDMSerBusiness::NewItemPops()
{
	std::shared_ptr<Node >node;


	return false;
}
