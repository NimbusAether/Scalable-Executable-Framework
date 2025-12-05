#pragma once
#include <map>
#include <list>
#include <vector>
#include <any>
#include "ConfigDefine.h"
#include "ByteArray.h"
#include "log.h"

#ifdef UDM_LIB
#define UDM_UDM_API UDM_EXPORT_API
#else
#define UDM_UDM_API UDM_IMPORT_API
#endif

using namespace SEF::Config;
using namespace SEF::COMMON;
using namespace SEF::Log;
namespace SEF
{
	namespace Udm
	{
		enum class  UdmEventType {
			UDM_EVENT_NONE = -1,
			UDM_EVENT_CONNECTED = 0,
			UDM_EVENT_CONNECTED_DELAY = 1,
			UDM_EVENT_CONNECT_RETRIED = 2,
			UDM_EVENT_DISCONNECTED = 3,
		};

		struct MessageInfo {
			SEF_String topicName;
			SEF_String variableName;
			SEF_String variableUnit;
			SEF_String sourceName;
			ValueType	type;
			SEF_Uint64 timestamp = 0;
			SEF_Bool localed = SEF_FALSE;
		};

		struct VarQueryInfo {
			SEF_String topicName;
			SEF_String varName;
			SEF_String var;
		};

		struct RequestQueryInfo {
			SEF_String topicName;
			SEF_String queueName;
			SEF_String requestName;
		};

		struct RequestInfo {
			SEF_String topicName;
			SEF_String queueName;
			SEF_String requestName;
		};
		class UdmEventCb {
		public:
			virtual ~UdmEventCb() {};
			virtual void OnEventRecv(const UdmEventType& type) = 0;
		};

		class NotifyCb {
		public:
			virtual ~NotifyCb() {};
			virtual void OnVarRecv(const MessageInfo& index, const std::any& value) = 0;
		};


		class RequestCb {
		public:
			virtual ~RequestCb() {};
			virtual void OnRequestRecv(const RequestInfo& index, const ByteArray& bytes) = 0;
		};

		class ConsumedVarInfoCb {
		public:
			virtual ~ConsumedVarInfoCb() {};
			virtual void OnConsumedVarInfoRecv(std::vector<MessageInfo> &vec) = 0;
		};

		class ProductedVarInfoCb {
		public:
			virtual ~ProductedVarInfoCb() {};
			virtual void OnProductedVarInfoRecv(std::vector<MessageInfo>& vec) = 0;
		};

		class RequestItemCb {
		public:
			virtual ~RequestItemCb() {};
			virtual void OnRequestItemRecv(std::vector<RequestInfo>& vec) = 0;
		};

		class UDM_UDM_API Udm
		{
		public:
			Udm();
			virtual ~Udm();
			virtual	bool init() = 0;
			virtual void read() = 0;
			virtual void commit() = 0;
			virtual bool publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool publish(SEF_Int64& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit = "n/a") { return false; };
			virtual bool publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit = "n/a") = 0;
			virtual bool publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool subscribe(SEF_Int64& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") { return false; };
			virtual bool subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit = "n/a") = 0;
			virtual bool subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName,  const SEF_String& variableUnit = "n/a") = 0;
			virtual bool subscribe(NotifyCb* cb, const VarQueryInfo& info) { return false; };
			virtual bool lock(const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit, std::any v) { return false; };
			virtual bool unlock(const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) { return false; };
			virtual void disableRead() {};


			virtual bool sendRequest(const COMMON::ByteArray &bytes, const RequestQueryInfo& info) = 0;
			virtual void setEventCb(UdmEventCb* cb) {};
			virtual bool recvRequest(const RequestQueryInfo& info, RequestCb *cb) = 0;
			virtual bool subscribeRequestQuery(RequestItemCb* cb) { return false; };
			virtual bool subscribeConsumedVarInfo(ConsumedVarInfoCb* cb) { return false; };
			virtual bool subscribeProductedVarInfo(ProductedVarInfoCb* cb) { return false; };
			virtual bool registerRequest(const RequestQueryInfo& info) { return false; };

			virtual bool unsubscribeNotify(const VarQueryInfo& info, NotifyCb* cb) { return false; };
			virtual bool unsubscribeNotify(NotifyCb* cb) { return false; };
			virtual bool unsubscribeRequest(const RequestQueryInfo& info) { return false; };

			SEF_String GetUdmName();
			UdmType GetUdmType();
			void log(const SEF_String &elementName, SEFLogLevel logType, const SEF_String &logMessage);
			void SetUdmName(const SEF_String &udmName);
			void SetConfigureFile(const SEF_String& file);
			void appendUdmAddress(const UdmAddress& info);
		protected:
			SEF_String udm_name;
			SEF_String udm_configFile;
			UdmType udm_type;
			std::list<UdmAddress> udm_address;
		};
	}
}