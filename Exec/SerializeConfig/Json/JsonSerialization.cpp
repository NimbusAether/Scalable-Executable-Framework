#include "JsonSerialization.h"
#include <fstream>
#include "json/json.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
class jsonContent{
public:
	Json::Value& getOrigalJsonValue();
	void setOrigalJsonValue(const Json::Value& value);

	Json::Value anyToJsonValue(const std::any& input) const;
	void jsonValueToAny(Json::Value intput, std::any& output) const;


public:
	Json::Value m_jsonValue;
};

JsonCppObjectAdapter::JsonCppObjectAdapter()
{
	m_content = new jsonContent();
}

JsonCppObjectAdapter::~JsonCppObjectAdapter()
{
	delete m_content;
}

bool JsonCppObjectAdapter::isBasicDataType(const std::any& data) const
{
	if (data.type() == typeid(char))
	{
		return true;
	}
	if (data.type() == typeid(int))
	{
		return true;
	}
	if (data.type() == typeid(float))
	{
		return true;
	}
	if (data.type() == typeid(double))
	{
		return true;
	}
	if (data.type() == typeid(short))
	{
		return true;
	}
	if (data.type() == typeid(long long))
	{
		return true;
	}
	if (data.type() == typeid(std::string))
	{
		return true;
	}
	if (data.type() == typeid(bool))
	{
		return true;
	}
	if (data.type() == typeid(unsigned short))
	{
		return true;
	}
	if (data.type() == typeid(unsigned int))
	{
		return true;
	}
	return false;
}

Json::Value jsonContent::anyToJsonValue(const std::any& data) const
{
	Json::Value value;
	if (data.type() == typeid(char))
	{
		value = std::any_cast<char>(data);
	}
	else if (data.type() == typeid(int))
	{
		value = std::any_cast<int>(data);
	}
	else if (data.type() == typeid(float))
	{
		value = std::any_cast<float>(data);
	}
	else if (data.type() == typeid(double))
	{
		value = std::any_cast<double>(data);
	}
	else if (data.type() == typeid(short))
	{
		value = std::any_cast<short>(data);
	}
	else if (data.type() == typeid(long long))
	{
		value = std::any_cast<long long>(data);
	}
	else if (data.type() == typeid(std::string))
	{
		value = std::any_cast<std::string>(data);
	}
	else if (data.type() == typeid(bool))
	{
		value = std::any_cast<bool>(data);
	}
	else if (data.type() == typeid(unsigned short))
	{
		value = std::any_cast<unsigned short>(data);
	}
	else if (data.type() == typeid(unsigned int))
	{
		value = std::any_cast<unsigned int>(data);
	}
	return value;
}

void jsonContent::jsonValueToAny(Json::Value intput, std::any& output) const
{
	if (output.type() == typeid(char))
	{
		output = intput.asBool();
	}
	else if (output.type() == typeid(int))
	{
		output = intput.asInt();
	}
	else if (output.type() == typeid(float))
	{
		output = intput.asFloat();
	}
	else if (output.type() == typeid(double))
	{
		output = intput.asDouble();
	}
	else if (output.type() == typeid(short))
	{
		output = (short)intput.asInt();
	}
	else if (output.type() == typeid(long long))
	{
		output = intput.asLargestInt();
	}
	else if (output.type() == typeid(std::string))
	{
		output = intput.asString();
	}
	else if (output.type() == typeid(bool))
	{
		output = intput.asBool();
	}
	else if (output.type() == typeid(unsigned short))
	{
		unsigned short v = intput.asUInt();
		output = v;
	}
	else if (output.type() == typeid(unsigned int))
	{
		output = intput.asUInt();
	}
}

void JsonCppObjectAdapter::writeValue(const std::string& key, const std::any& data)
{
	Json::Value value =	m_content->anyToJsonValue(data);
	if (!value.isNull())
	{
		m_content->m_jsonValue[key] = value;
	}
}

void JsonCppObjectAdapter::readValue(const std::string& key, std::any& data) const
{
	m_content->jsonValueToAny(m_content->m_jsonValue[key], data);
}

void JsonCppObjectAdapter::writeArray(const std::string& key, const std::vector<std::any>& datas)
{
	if (datas.empty())
	{
		return;
	}
	Json::Value array;
	if (isBasicDataType(datas[0]))
	{
		for (size_t i = 0; i < datas.size(); i++)
		{
			array.append(m_content->anyToJsonValue(datas[i]));
		}
	}
	else
	{
		for (size_t i = 0; i < datas.size(); i++)
		{
			auto child = std::any_cast<std::shared_ptr<SerializableObject>>(datas[i]);
			array.append(static_cast<JsonCppObjectAdapter*>(child.get())->m_content->getOrigalJsonValue());
		}
	}

	m_content->m_jsonValue[key] = array;
}

void JsonCppObjectAdapter::readArray(const std::string& key, std::vector<std::any>& datas, std::any feature) const
{
	Json::Value array = m_content->m_jsonValue[key];
	for (auto i = 0; i < array.size(); i++)
	{
		Json::Value jsonValue = array[i];
		if (jsonValue.isObject())
		{
			auto child = std::make_shared<JsonCppObjectAdapter>();
			child->m_content->setOrigalJsonValue(jsonValue);
			datas.push_back(std::shared_ptr<SerializableObject>(child));
		}
		else
		{
			std::any data = feature;
			m_content->jsonValueToAny(jsonValue, data);
			datas.push_back(data);
		}
	}
}

Json::Value& jsonContent::getOrigalJsonValue()
{
	return m_jsonValue;
}

void jsonContent::setOrigalJsonValue(const Json::Value& value)
{
	m_jsonValue = value;
}

void JsonCppObjectAdapter::updateChildObject(const std::string& key, std::shared_ptr<SerializableObject> child)
{
	m_content->m_jsonValue[key] = (static_cast<JsonCppObjectAdapter*>(child.get())->m_content->getOrigalJsonValue());
}

std::shared_ptr<SerializableObject> JsonCppObjectAdapter::childObject(const std::string& key) const
{
	if (!m_content->m_jsonValue[key].isNull())
	{
		auto child = std::make_shared<JsonCppObjectAdapter>();
		child->m_content->setOrigalJsonValue(m_content->m_jsonValue[key]);
		return child;
	}
	return nullptr;
}

std::shared_ptr<SerializableObject> JsonCppObjectAdapter::createEmptyObject() const
{
	return std::make_shared<JsonCppObjectAdapter>(); 
}

std::string JsonCppObjectAdapter::toString()
{
	return m_content->m_jsonValue.toStyledString();
}

void JsonCppObjectAdapter::fromString(const std::string& json)
{
	Json::Reader reader;
	bool ret = reader.parse(json.data(), m_content->m_jsonValue);
	if (ret == false) {
		throw "please check json string:\n" + json;
	}
}

void JsonCppObjectAdapter::fromFile(const std::string& filename)
{
	Json::CharReaderBuilder builder;
	std::string errs;
	if (filename.empty() == false) {
		std::ifstream ifs(filename);
		if (ifs.is_open() == false) {
			throw "open " + filename + " failed";
		}
		if (!Json::parseFromStream(builder, ifs, &m_content->m_jsonValue, &errs)) {
			throw "parser " + filename + " failed:"+errs;
		}
	}
	else {
		throw "empty filename";
	}
}