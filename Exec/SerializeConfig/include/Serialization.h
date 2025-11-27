/*
* Copyright 
* All rights reserved.
*
* 文件名称：Serialization.h
* 摘   要：序列化相关模板
* 说   明：
* 日   期: 2024.9
* 版   本: 1.0.0
* 作者/修改者：cuizhilei
*/
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <vector>
#include <string>
#include <any>
#include <memory>

#define UDM_EXPORT_API __declspec(dllexport)
#define UDM_IMPORT_API __declspec(dllimport)

#ifdef SERIALIZE_LIB
#define UDM_SERIALIZE_API UDM_EXPORT_API
#else
#define UDM_SERIALIZE_API UDM_IMPORT_API
#endif

class UDM_SERIALIZE_API SerializableObject
{
public:
    SerializableObject() {}
    virtual ~SerializableObject() {}

    virtual bool isBasicDataType(const std::any& data) const = 0;

    virtual void writeValue(const std::string& key, const std::any& data) = 0;
    virtual void readValue(const std::string& key, std::any& data) const = 0;

    virtual void updateChildObject(const std::string& key, std::shared_ptr<SerializableObject> child) = 0;
    virtual std::shared_ptr<SerializableObject> childObject(const std::string& key) const = 0;
    virtual std::shared_ptr<SerializableObject> createEmptyObject() const = 0;
    virtual void writeArray(const std::string& key, const std::vector<std::any>& datas) = 0;

    //由于datas数组传入的是空数组，所以成员数据类型无法得知，专门传入一个defaultValue，用来内部识别数据类型
    virtual void readArray(const std::string& key, std::vector<std::any>& datas, std::any feature) const = 0;

    virtual std::string toString() = 0;
    virtual void fromString(const std::string& json) = 0;
    virtual void fromFile(const std::string& filename) = 0;
};

/*----------------------------------------------------------------
* 交换对象的成员函数模板。自定义数据类型，需要特化此模板
*---------------------------------------------------------------*/
template<typename ValueType>
void swapMember(ValueType&, SerializableObject*, bool) {}

/*----------------------------------------------------------------
* 读写SerializeObject的模板函数
*---------------------------------------------------------------*/
template<typename ValueType>
void saveToObject(const std::string &key, const ValueType &data, SerializableObject* object)
{
    if (object->isBasicDataType(data))
    {
        object->writeValue(key, data);
    }
    else
    {
        auto child = object->createEmptyObject();
        swapMember(const_cast<ValueType&>(data), child.get(), true);
        object->updateChildObject(key, child);
    }
}

template<typename ValueType>
void loadFromObject(const std::string &key, ValueType &data, const SerializableObject* object)
{
    if (object->isBasicDataType(data))
    {
        std::any tempData = data;
        object->readValue(key, tempData);
        data = std::any_cast<ValueType>(tempData);
    }
    else
    {
        auto child = object->childObject(key);
        if (child != nullptr)
        {
            swapMember(const_cast<ValueType&>(data), child.get(), false);
        }
    }
}

template<typename ValueType>
void saveToObject(const std::string& key, const std::vector<ValueType>& datas, SerializableObject* object)
{
    if (datas.empty())
    {
        return;
    }
    std::vector<std::any> tempVector;
    if (object->isBasicDataType(datas[0]))
    {
        for (size_t i = 0; i < datas.size(); i++)
        {
            tempVector.push_back(datas[i]);
        }
    }
    else
    {
        for (size_t i = 0; i < datas.size(); i++)
        {
            auto child = object->createEmptyObject();
            swapMember(const_cast<ValueType&>(datas[i]), child.get(), true);
            tempVector.push_back(child);
        }
    }
    object->writeArray(key, tempVector);
}

template<typename ValueType>
void loadFromObject(const std::string& key, std::vector<ValueType>& datas, const SerializableObject* object)
{
    std::vector<std::any> tempVector;
    object->readArray(key, tempVector, ValueType());
    if (tempVector.empty())
    {
        return;
    }
    if (object->isBasicDataType(tempVector[0]))
    {
        for (size_t i = 0; i < tempVector.size(); i++)
        {
            datas.push_back(std::any_cast<ValueType>(tempVector[i]));
        }
    }
    else
    {
        for (size_t i = 0; i < tempVector.size(); i++)
        {
            ValueType data;
            std::shared_ptr<SerializableObject> child = std::any_cast<std::shared_ptr<SerializableObject>>(tempVector[i]);
            swapMember(data, child.get(), false);
            datas.push_back(data);
        }
    }
}

/*----------------------------------------------------------------
* 与serializableObject交换的模板函数
*---------------------------------------------------------------*/
template<typename ValueType>
void swapWithObject(const std::string &key, ValueType &data, SerializableObject* serializableObject, bool isForWrite)
{
    if (isForWrite)
    {
        saveToObject(key, data, serializableObject);
    }
    else
    {
        loadFromObject(key, data, serializableObject);
    }
}

template<typename ValueType, typename SerializableObjectType>
std::string convertToString(const ValueType& data)
{
    SerializableObjectType object;
    swapMember(const_cast<ValueType&>(data), &object, true);
    return object.toString();
}

template<typename ValueType, typename SerializableObjectType>
ValueType convertFromString(const std::string& json)
{
    SerializableObjectType object;
    object.fromString(json);
    ValueType data;
    swapMember(data, &object, false);
    return data;
}

template<typename ValueType, typename SerializableObjectType>
ValueType convertFromFile(const std::string& filename)
{
    SerializableObjectType object;
    object.fromFile(filename);
    ValueType data;
    swapMember(data, &object, false);
    return data;
}

#define SERIALIZE(CustomType) \
    template void swapWithObject<CustomType>(const std::string& key, CustomType &data, SerializableObject* object, bool isForWrite); \
    template<> inline void swapMember(CustomType& data, SerializableObject* object, bool isForWrite)

#define SWAP(name) swapWithObject(#name, data.name, object, isForWrite)
#endif
