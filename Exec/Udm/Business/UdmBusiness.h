#pragma once
#include <thread>
#include <map>
#include <list>
#include "NetUdm.h"
#include "UdmJsonRefMsg.h"
#include "base.h"
#include "internal.h"
class BusContent;

namespace SEF
{
	namespace Udm
	{	

		class UdmBusiness:public NetUdm
		{
		public:
			UdmBusiness();
			virtual ~UdmBusiness();

			/*
			* 通过手动推送commit/手动更新read 数据
			*/
			void read() override;
			void commit() override;
			bool publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool publish(SEF_Int64& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit = "am");
			bool publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool subscribe(SEF_Int64& input, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit = "am");
			bool subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool lock(const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit, std::any v) override;
			bool unlock(const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			void disableRead() override;


			bool sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)override;

			/*
			* 实时订阅通知接口，通过回调实时通知，目前订阅消息分3大类：
			* 1、topic/variable/value
			* 2、topic/request
			* 3、toipc目录
			*/
			bool subscribe(NotifyCb* cb, const VarQueryInfo& info)override;
			//bool subscribeRequest(const QueryInfo& info, RequestCb * cb)override;
			
			bool recvRequest(const RequestQueryInfo& info, RequestCb * cb)override;
			bool subscribeRequestQuery(RequestItemCb* cb) override;
			bool subscribeConsumedVarInfo(ConsumedVarInfoCb* cb) override;
			bool subscribeProductedVarInfo(ProductedVarInfoCb* cb) override;


			bool unsubscribeNotify(const VarQueryInfo& info, NotifyCb* cb) override;
			bool unsubscribeNotify(NotifyCb* cb)override;
			bool unsubscribeRequest(const RequestQueryInfo& info) override;

			bool registerRequest(const RequestQueryInfo& info) override;


		protected:
			/*
			* 通过socket发布消息
			*/
			virtual bool publish(std::shared_ptr<UdmMessage> udmMessage) = 0;
			/*
			* 通过socket订阅消息
			*/
			virtual bool subscribe(const SEF_String& topic) = 0;
			/*
			* 通过socket订阅消息
			*/
			virtual bool unsubscribe(const SEF_String& topic) = 0;
			/*
			* 通过socket发送Request并同步返回响应数据
			*/
			virtual bool _sendRequest(UdmMessage& message, UdmMessage& reply) = 0;

			/*
			* 通过socket推送topic/variableName对消息
			*/
			virtual bool pushSubItem(UdmMessage& udmMessage) = 0;

			virtual bool isUdmConnected() = 0;
		private:
			/*
			* 消息目录订阅以及全量获取
			*/
			bool subscribeItemDir(INTERNAL::TreeItemType type, const SEF_String& topic ,ConsumedVarInfoCb* cb);
			bool subscribeItemDir(INTERNAL::TreeItemType type, const SEF_String& topic, ProductedVarInfoCb* cb);


			bool subscribeRequestItemDir(INTERNAL::TreeItemType toicpQueryType, const SEF_String& topic, RequestItemCb* cb);

			/*
			* 接受订阅服务发布的消息
			*/
			void NotifyVariable(const MessageInfo& index, std::any value);

			void NotifyTopicItem(std::shared_ptr<UdmMessage> msg);

			//void buildVarItemFromMsg(std::vector<MessageInfo>& vec, topicList *list);
			//void buildReqItemFromMsg(std::vector<RequestInfo>& vec, topicList *list);


			void NotifyRequest(UdmMessage& msg);

			bool manualCommit(ValueType type, SEF_String& value, size_t valueSize, std::shared_ptr<UdmMsg> msg);

			bool buildSubItem(ValueType nodeType, const SEF_String& topic, const SEF_String& variableName, const SEF_String& variableUnit, UdmMessage& udmMessage, INTERNAL::TreeItemType itemType);
			/*
			* threads
			*/
			void notifyThreadCycle();
			void commitThreadCycle();
		protected:
			void onRecv(std::shared_ptr<UdmMessage> message);

		private:
			BusContent* m_ctx;
		};
	}
}