#pragma once
#include "UdmMessage.h"
#include "UdmService.h"
#include "SafeQueue.h"
#include <map>
#include <list>
#include <mutex>
#include "UdmMessage.h"
#include "ByteArray.h"
#include "log.h"
#include "topicList.pb.h"
#include "internal.h"
using namespace SEF::Udm;

namespace SEF
{
	namespace Udm
	{
		enum class ItemType {
			REQUEST_ITEM = 0,
			VARIABLE_ITEM = 1,
		};


		class MQNotifier {
		public:
			virtual void onCommonMessage(std::shared_ptr <UdmMessage> message)const = 0;
		};

		class UDMSerBusiness
		{
		public:
			static UDMSerBusiness* getInstance();
			static void freeInstance();

			void addNotiify(const MQNotifier *nofitier);
			bool flushItemLockedStatus(std::shared_ptr<UdmMessage> message, SEF_Bool locked);
			bool onRecv(std::shared_ptr<UdmMessage> message);
			bool onRequestAllItems(UdmMessage& request, UdmMessage &response);
			bool onRequestAllItems(UdmMessage& request, int step, std::list<UdmMessage> &list);
		private:
			static UDMSerBusiness* instance;
			UDMSerBusiness();
			~UDMSerBusiness();
			void publishThreadCycle();
			void ItemThreadCycle();
			bool NewItemPops();

		private:
			std::list<const MQNotifier*>m_notifierList;
			std::mutex m_mutex;
			std::thread pPublishThd;
			std::thread pItemThd;

			std::mutex m_itemsMutex;

			std::mutex m_itemsNewMutex;
			std::condition_variable m_itemsNewCond;


			map<string, std::shared_ptr<UDM::Node>> m_RequestItems;
			SafeQueue<std::shared_ptr<UDM::Node>> m_NewRequestItems;

			map<string, std::shared_ptr<UDM::Node>> m_VariableSubItemMap;
			SafeQueue<std::shared_ptr<UDM::Node>> m_NewVariableSubItemQueue;

			map<string, std::shared_ptr<UDM::Node>> m_VariablePubItemMap;
			SafeQueue<std::shared_ptr<UDM::Node>> m_NewVariablePubItemQueue;

			SafeQueue<std::shared_ptr<UdmMessage>> m_allQueue;
			map<string, std::shared_ptr<UdmMessage>> m_LockMsgMap;
			bool m_running;


		};
	}
}