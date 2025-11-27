#pragma once
#include "UdmBusiness.h"
#include <thread>
#include "UdmMessage.h"
class ZeroContentEx;

namespace SEF
{
	namespace Udm
	{	
		class UDM_UDM_API ZeroMQUdmEx : public UdmBusiness
		{
		public:
			ZeroMQUdmEx();
			virtual ~ZeroMQUdmEx();
			bool init() override;
		protected:
			/*
			* 通过socket发布消息
			*/
			bool publish(std::shared_ptr<UdmMessage> udmMessage) override;

			/*
			* 通过socket发送Request并同步返回响应数据
			*/
			bool _sendRequest(UdmMessage& message, UdmMessage& reply) override;

			/*
			* 通过socket订阅消息
			*/
			bool subscribe(const string& topic) override;

			bool unsubscribe(const string& topic) override;

			/*
			* 通过socket推送topic/variableName对消息
			*/
			bool pushSubItem(UdmMessage& udmMessage) override;

			void setEventCb(UdmEventCb* cb) override;

			bool isUdmConnected() override;


		private:
			bool ZeroContextInit();
			void zerothreadCycle();
			void MonitorCycle();

		private:
			ZeroContentEx*	m_ctx = nullptr;
			std::thread*	m_thread = nullptr;
			std::thread*	m_MonitorThread = nullptr;
			UdmEventCb*		p_eventCb = nullptr;
		};
	}
}