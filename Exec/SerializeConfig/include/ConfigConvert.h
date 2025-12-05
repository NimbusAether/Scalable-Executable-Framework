#pragma once
#include <string>
#include "JsonSerialization.h"

#define CONFIG_SERIALIZE(CustomType) SERIALIZE(CustomType)
#define CONFIG_SWAP(name) SWAP(name)

/* ≈‰÷√ æ¿˝
* struct configBase{
*	int			a;
*	unsigned int	b;
*	std::string	c;
* }
* CONFIG_SERIALIZE(configBase)
* {
*	CONFIG_SWAP(a);
*	CONFIG_SWAP(b);
*	CONFIG_SWAP(c);
* }
* 
*
* struct config{
*	vector<configBase> vc;
*	int		t;
*	float   c;
* }
*
* CONFIG_SERIALIZE(config)
* {
*	CONFIG_SWAP(vc);
*	CONFIG_SWAP(t);
*	CONFIG_SWAP(c);
* }
*  ConfigConvert<config> handle(ConvertType::Json)
* config c = handle.configConvertFromFile(filepath)
* config c = handle.configConvertFromString(string)
* string str = handle.configConvertToString(c)
* 
*/

enum ConvertType
{
	NONE = -1,
	JSON = 0
};

template<typename T>
class ConfigConvert {
public:
	ConfigConvert(ConvertType type) :m_type(type) {};
	~ConfigConvert(){}
	std::string configConvertToString(const T& value);
	T configConvertFromString(const std::string& value);
	T configConvertFromFile(const std::string& filaname);

private:
	ConvertType m_type;

};
template<typename T>
inline std::string ConfigConvert<T>::configConvertToString(const T& value)
{
	if (m_type == ConvertType::JSON) 
	{
		return convertToString<T, JsonCppObjectAdapter>(value);
	}
	else 
	{
		return std::string();
	}
}

template<typename T>
inline T ConfigConvert<T>::configConvertFromString(const std::string& value)
{
	if (m_type == ConvertType::JSON)
	{
		return convertFromString<T, JsonCppObjectAdapter>(value);
	}
	else 
	{
		return T();
	}
}

template<typename T>
inline T ConfigConvert<T>::configConvertFromFile(const std::string& filaname)
{
	if (m_type == ConvertType::JSON) 
	{
		return convertFromFile<T, JsonCppObjectAdapter>(filaname);
	}
	else 
	{
		return T();
	}
}
