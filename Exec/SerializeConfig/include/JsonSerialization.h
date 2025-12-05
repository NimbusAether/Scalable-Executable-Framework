#pragma once
#include "Serialization.h"



class jsonContent;

class UDM_SERIALIZE_API JsonCppObjectAdapter : public SerializableObject
{
public:
	JsonCppObjectAdapter();
	~JsonCppObjectAdapter();

	bool isBasicDataType(const std::any& data) const override;

	void writeValue(const std::string& key, const std::any& data) override;
	void readValue(const std::string& key, std::any& data) const override;

	void updateChildObject(const std::string& key, std::shared_ptr<SerializableObject> child) override;
	std::shared_ptr<SerializableObject> childObject(const std::string& key) const override;
	std::shared_ptr<SerializableObject> createEmptyObject() const override;

	void writeArray(const std::string& key, const std::vector<std::any>& datas) override;
	void readArray(const std::string& key, std::vector<std::any>& datas, std::any feature) const override;

	std::string toString() override;
	void fromString(const std::string& json) override;
	void fromFile(const std::string& filename) override;

private:
	jsonContent* m_content;
};


//template<typename T>
//std::string convertToJsonString(const T& data)
//{
//	return convertToString<T, JsonCppObjectAdapter>(data);
//}
//
//template<typename T>
//T convertFromJsonString(const std::string& json)
//{
//	return convertFromString<T, JsonCppObjectAdapter>(json);
//}
//
//template<typename T>
//T convertFromJsonFile(const std::string& filaname)
//{
//	return convertFromFile<T, JsonCppObjectAdapter>(filaname);
//}

