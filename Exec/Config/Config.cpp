#include "Config.h"
#include "ConfigConvert.h"
using namespace SEF::Config;
#ifdef VLD_DEBUG
#include "vld.h"
#endif

CONFIG_SERIALIZE(ElementInfo)
{
	CONFIG_SWAP(name);
	CONFIG_SWAP(type);
	CONFIG_SWAP(configFile);
	CONFIG_SWAP(dllFile);
}
CONFIG_SERIALIZE(GroupInfo)
{
	CONFIG_SWAP(name);
	CONFIG_SWAP(frequency);
	CONFIG_SWAP(elements);
}
CONFIG_SERIALIZE(UdmAddress)
{
	CONFIG_SWAP(protocol);
	CONFIG_SWAP(ip);
	CONFIG_SWAP(port);
	CONFIG_SWAP(isLocal);
}
CONFIG_SERIALIZE(UdmInfo)
{
	CONFIG_SWAP(type);
	CONFIG_SWAP(address);
}
CONFIG_SERIALIZE(LogConfig)
{
	CONFIG_SWAP(level);
	CONFIG_SWAP(path);
}
CONFIG_SERIALIZE(SEFConfig)
{
	CONFIG_SWAP(kernelName);
	CONFIG_SWAP(prefixPath);
	CONFIG_SWAP(frequency);
	CONFIG_SWAP(realTimeDevice);
	CONFIG_SWAP(schedPolicy);
	CONFIG_SWAP(kernelThreadPriority);
	CONFIG_SWAP(hardwareScope);
	CONFIG_SWAP(startupTimeout);
	CONFIG_SWAP(publishTimeout);
	CONFIG_SWAP(subscribeTimeout);
	CONFIG_SWAP(initializeTimeout);
	CONFIG_SWAP(shutdownTimeout);
	CONFIG_SWAP(snapshotTimeout);

	CONFIG_SWAP(logInfo);
	CONFIG_SWAP(udmServiceInfo);
	CONFIG_SWAP(udmClientInfo);
	CONFIG_SWAP(elementGroup);
}

std::string addPrefixPath(const std::string& prefix, const std::string file)
{
	return prefix + "\/" + file;
}
Config::Config() 
{

	config_type = ConfigType::NONE;
}

Config::Config(ConfigType type)
{
	config_type = type;
}

Config::~Config() 
{

}
const std::vector<GroupInfo>& Config::GetGroupInfo()
{
	return m_config.elementGroup;
}

const std::vector<UdmInfo>& Config::GetUdmServiceConfig()
{
	return m_config.udmServiceInfo;
}

const UdmInfo& Config::GetUdmClientConfig()
{
	return m_config.udmClientInfo;
}


void Config::openConfigureFile(const COMMON::SEF_String& configureString, ConfigType type)
{
	m_configStr = configureString;
	ConvertType cType = ConvertType::NONE;
	switch (type) 
	{
		case ConfigType::JSON: 
		{
			cType = ConvertType::JSON;
			break;
		}
		default:
			throw "unsupport configure file type";
	}

	ConfigConvert<SEFConfig> conv(cType);
	try 
	{
		m_config = conv.configConvertFromFile(m_configStr);
	}
	catch (std::string errs) 
	{
		throw "Config error:" + errs;
	}
	m_config.logInfo.path = addPrefixPath(m_config.prefixPath, m_config.logInfo.path);
	m_config.udmClientInfo.configFile = addPrefixPath(m_config.prefixPath, m_config.udmClientInfo.configFile);
	for (int i = 0;i<m_config.udmServiceInfo.size();i++)
	{
		m_config.udmServiceInfo[i].configFile = addPrefixPath(m_config.prefixPath, m_config.udmServiceInfo[i].configFile);
	}
	for (int i = 0;i<m_config.elementGroup.size();i++)
	{
		for (int j = 0;j< m_config.elementGroup[i].elements.size();j++)
		{
			m_config.elementGroup[i].elements[j].configFile = addPrefixPath(m_config.prefixPath, m_config.elementGroup[i].elements[j].configFile);
			m_config.elementGroup[i].elements[j].dllFile = addPrefixPath(m_config.prefixPath, m_config.elementGroup[i].elements[j].dllFile);
		}
	}
}

const SEFConfig& SEF::Config::Config::GetSEFConfig()
{
	return m_config;
}

const LogConfig& SEF::Config::Config::GetLogConfig()
{
	return m_config.logInfo;
}


