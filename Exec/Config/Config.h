#pragma once
#include "ConfigDefine.h"
#include <map>
#include <string>
#include <vector>

namespace SEF {
	namespace Config {
		struct SEFConfig {
			std::string		prefixPath;
			std::string		kernelName;
			int				frequency;
			bool			realTimeDevice;
			int				schedPolicy;
			int				kernelThreadPriority;
			bool			hardwareScope;
			int		startupTimeout;
			int		publishTimeout;
			int		subscribeTimeout;
			int		initializeTimeout;
			int		shutdownTimeout;
			int		snapshotTimeout;

			LogConfig	logInfo;
			std::vector<UdmInfo> udmServiceInfo;
			UdmInfo				udmClientInfo;
			std::vector<GroupInfo> elementGroup;

			SEFConfig() {
				frequency = 0;
				realTimeDevice = false;
				schedPolicy = 0;
				kernelThreadPriority = 0;
				hardwareScope = false;
				startupTimeout = 0;
				publishTimeout = 0;
				subscribeTimeout = 0;
				initializeTimeout = 0;
				shutdownTimeout = 0;
				snapshotTimeout = 0;
			}
		};


		class UDM_EXPORT_API Config
		{
		public:
			Config(ConfigType type);
			Config();
			~Config();
			void openConfigureFile(const COMMON::SEF_String &configureString, ConfigType type = ConfigType::JSON);
			const std::vector<GroupInfo>& GetGroupInfo();
			const std::vector<UdmInfo>& GetUdmServiceConfig();
			const UdmInfo& GetUdmClientConfig();
			const SEFConfig& GetSEFConfig();
			const LogConfig& GetLogConfig();
		protected:
			ConfigType config_type;
			std::string m_configStr;
			SEFConfig m_config;
		};
	}
}