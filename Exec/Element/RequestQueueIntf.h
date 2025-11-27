#pragma once
#include "UdmRequestQueue.h"
#include <map>

namespace SEF {
	namespace Element {
		inline const char* s_defaultQueueName = "defaultQueue";
		class UDM_EXPORT_API RequestQueueIntf
		{
		public:
			struct RequestQueueArgs
			{
				SEF::COMMON::SEF_String				topicName;
				SEF::COMMON::SEF_String				queueName;
				std::shared_ptr<SEF::COMMON::Request>	request;
			};
			typedef void (RequestQueueIntf::*RequestHandler)(const RequestQueueArgs&);
		protected:
			RequestQueueIntf();
			virtual ~RequestQueueIntf();
			/*
			 * This overloaded version of the registerQueueHandler uses the
			 * default queue name "Default".
			 */
			void registerQueue(const SEF::COMMON::SEF_String& topic,const SEF::COMMON::SEF_String& queueName);
			void registerQueueHandler(const SEF::COMMON::SEF_String& topic, const SEF::COMMON::SEF_String& queueName,
											const SEF::COMMON::SEF_String& requestName, RequestHandler handler);
			void registerQueueHandler(const SEF::COMMON::SEF_String& topic, const SEF::COMMON::SEF_String& requestName, RequestHandler handler);

			/*
			*  Notification Function of a pop change to a particular queue.
			*/
			void requestQueuePopped(const SEF::COMMON::SEF_String& topic, const SEF::COMMON::SEF_String& queueName,
										SEF::COMMON::RequestQueue* queue, const SEF::COMMON::Request& request, const SEF::COMMON::SEF_Bool handled);

			/*
			* Notification Function of a push change to a particular queue.
			*/
			void requestQueuePushed(const SEF::COMMON::SEF_String& topic, const SEF::COMMON::SEF_String& queueName,
										SEF::COMMON::RequestQueue* queue, const SEF::COMMON::Request& request);

			void setName(const SEF::COMMON::SEF_String& name);
			/*
			 * Creates a default request queue and adds it to the local Element map.
			 * The default request queue's name is "Default".
			 */
			void registerQueue(const SEF::COMMON::SEF_String& topic);

			/** Handles new messages on the request queues. All handled requests
			* will be popped (removed from their queues and deleted.
			* @return:
			* -true: Handled at least one request.
			* -false: No requests were handled.
			* @param handleAll
			* -true: Loop through all queues and handle all existing requests on all queues.
			* -false: Only handle max one requests per queue before returning.
			*/
			virtual SEF::COMMON::SEF_Bool handleRequestQueues(SEF::COMMON::SEF_Bool handleAll = true);

			virtual void handleRequestQueue(const SEF::COMMON::SEF_String& topicName,
												const SEF::COMMON::SEF_String& queueName, const SEF::COMMON::SEF_Bool handleAll = true);

			virtual SEF::COMMON::SEF_Bool registerRequest(const SEF::COMMON::SEF_String& topicName, const SEF::COMMON::SEF_String& queueName, const SEF::COMMON::SEF_String& request) = 0;

			/*
			* Removes all requests from a particular queue
			*/
			void clearQueue(const SEF::COMMON::SEF_String& topic, const SEF::COMMON::SEF_String& queueName);
			/*
			* Removes all requests from all queues
			*/
			void clearAllQueues();

			/*
			* Removes all requests from all queues
			*/
			SEF::COMMON::SEF_Bool pushRequest(const SEF::COMMON::SEF_String& topicName, const SEF::COMMON::SEF_String& queueName, const SEF::COMMON::ByteArray& requestBytes);
			SEF::COMMON::SEF_Bool pushRequest(const SEF::COMMON::SEF_String& topicName,  const SEF::COMMON::ByteArray& requestBytes);
		protected:
			struct RequestQueueData
			{
				SEF::COMMON::RequestQueue* queue = nullptr;
				std::map<SEF::COMMON::SEF_String, RequestHandler> handlers;
			};
			typedef std::map<SEF::COMMON::SEF_String, RequestQueueData*>  QueueNameMap;

			std::map<SEF::COMMON::SEF_String, QueueNameMap*>         m_requestQueueSet;
			SEF::COMMON::SEF_String m_name;

		private:
			QueueNameMap* match(const SEF::COMMON::SEF_String& topic);
			RequestQueueData* match(const SEF::COMMON::SEF_String& topic, const SEF::COMMON::SEF_String& queueName);
			SEF::COMMON::SEF_Int handleRequestQueueData(const SEF::COMMON::SEF_String& topicName, RequestQueueData* requestQueueData, SEF::COMMON::SEF_Bool handleAll);
		};
	}
}