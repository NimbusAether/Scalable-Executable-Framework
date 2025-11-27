#pragma once
#include <map>
#include <string>
#include <vector>
#include "base.h"



namespace SEF {
	namespace Config {

		enum ConfigType
		{
			NONE = -1,
			JSON = 0,
		};

		enum class DirectType
		{
			LOCAL = 1,
			REMOTE = 0,
		};

		enum class RoleType
		{
			None = -1,
			/*TO zeroMq*/
			PUBLISH = 0,
			SUBSCRIBE = 1,
			PULL = 2,
			PUSH = 3,
			REQUEST = 4,
			RESPONSE = 5,

			/*to socketUdm*/
			BIND = 6,
			CONNECT = 7,
		};
		enum class ProtocolType {
			PROTOCOL_TYPE_NONE = -1,
			PROTOCOL_TYPE_TCP = 0,
			PROTOCOL_TYPE_UDP = 1,
			PROTOCOL_TYPE_IPC = 2,
		};

		struct ElementInfo {
			std::string name;
			std::string type;
			std::string configFile;
			std::string dllFile;
		};

		struct GroupInfo {
			std::string name;
			COMMON::SEF_Int32  frequency;
			std::vector<ElementInfo> elements;

			GroupInfo() :frequency(0) {}
		};

		struct UdmAddress {
			COMMON::SEF_Int32 protocol;
			std::string ip;
			unsigned short port;
			bool	isLocal;
			UdmAddress() :protocol(0), isLocal(0), port(0) {}
		};

		struct UdmInfo {
			COMMON::SEF_Int32 type;
			COMMON::SEF_String configFile;
			std::vector<UdmAddress> address;
			UdmInfo() :type(0) {}
		};

		struct LogConfig {
			COMMON::SEF_Int32 level;
			std::string path;
			LogConfig() :level(0) {}
		};
	}
}