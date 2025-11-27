#include "Mode.h"
#include "Element1.h"
#include "UdmFactory.h"
 // STL and external lib headers next
#include <iostream>
#include <ctime>
#include <string>
#include <chrono>
#include <cstdlib>
#include "ZeroMQUdmFactory.h"
#include "UdmRequestQueue.h"
#include "UdmFactoryGenerator.h"
#include "internal.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using namespace SEF;
using namespace SEF::Udm;
using namespace SEF::COMMON;
using namespace std;
//using namespace SEF::Elements;
using SEF::Elements::Element1::Element1;
using  SEF::Udm::Udm;
using SEF::Element::RequestQueueIntf;

#define SAFE_DELETE(p) \
do {\
if(p) delete p;\
p = nullptr;\
}while(0)\

static int logDur = 500;


class RequestSubPublish : public SEF::Udm::RequestCb {
public:
    RequestSubPublish(SEF::Udm::Udm * udm) : SEF::Udm::RequestCb(), m_udm(udm)
    {
    
    }
    ~RequestSubPublish()
    {
    }
    bool addSubscriber(const std::string &topic)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        RequestQueryInfo info;
        info.topicName = topic;
        return m_udm->recvRequest(info, this);
    }
    bool publishString(const std::string& topic, const std::string& msg)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ByteArray bytes(msg);
        RequestQueryInfo info;
        info.topicName = topic;
        return m_udm->sendRequest(bytes, info);
    }

    virtual void OnRequestRecv(const RequestInfo& topicName, const ByteArray& bytes) = 0;
public:
    mutable std::mutex m_mutex;

    SEF::Udm::Udm* m_udm = nullptr;
};


inline static SEF_Uint64 getMillsec()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto millseconds = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    SEF_Uint64 value = millseconds.time_since_epoch().count();
    return value;
}


void SEF::Elements::Element1::Element1::OnVarRecv(const MessageInfo& Index, const std::any& value) {
    if (Index.localed == SEF_TRUE)
    {
        int j = 1;
    }
    if (value.type() == typeid(bool)) {

        bool res = std::any_cast<bool>(value);
        logTrace("NotifyCb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res)+" lock:"+to_string(Index.localed));

    }
    else if (value.type() == typeid(int))
    {
        int res = std::any_cast<int>(value);

        logTrace("NotifyCb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res) + " lock:" + to_string(Index.localed));

    }
    else if (value.type() == typeid(SEF_Int64))
    {
        int64_t res = std::any_cast<int64_t>(value);

        logTrace("NotifyCb INT64: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res) + " lock:" + to_string(Index.localed));

    }
    else if (value.type() == typeid(float))
    {
        float res = std::any_cast<float>(value);
        logTrace("NotifyCb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res) + " lock:" + to_string(Index.localed));

    }
    else if (value.type() == typeid(double))
    {
        double res = std::any_cast<double>(value);
        logTrace("NotifyCb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res) + " lock:" + to_string(Index.localed));

    }
    else if (value.type() == typeid(string))
    {
        std::string str = std::any_cast<std::string>(value);
        logTrace("NotifyCb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + str + " lock:" + to_string(Index.localed));
    }

    if (Index.variableName == "status")
    {
        int res = std::any_cast<int>(value);
        logDebug("Notify element status Cb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res) + " lock:" + to_string(Index.localed));
    }
    else if (Index.variableName == "failedCount")
    {
        int res = std::any_cast<int>(value);
        logDebug("Notify element failedCount Cb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res) + " lock:" + to_string(Index.localed));
    }
    else if (Index.variableName == "successedCount")
    {
        int res = std::any_cast<int>(value);
        logDebug("Notify element successedCount Cb: " + Index.topicName + " " + Index.variableName + " " + Index.variableUnit + " " + to_string(res) + " lock:" + to_string(Index.localed));
    }

}


void SEF::Elements::Element1::Element1::OnEventRecv(const UdmEventType& type)
{
    if (type == UdmEventType::UDM_EVENT_CONNECTED)
    {
        isUdmConntected = true;

        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.notify_one();
    }
    else if (type == UdmEventType::UDM_EVENT_DISCONNECTED)
    {
        isUdmConntected = false;

        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.notify_one();
    }
}






static void onRecvRequestCb(const string& topicName, const ByteArray& bytes, void* arg)
{


}

void Element1::onRequestHandler(const RequestQueueArgs&arg)
{
    for (RequestTopic requestTopic : m_config.requestItems.subscribe)
    {

    }

}

bool SEF::Elements::Element1::Element1::PublishOutputsLock(bool locked)
{
    if (locked == true)
    {
        for (VarInt& value : m_config.outputs.intVec) {
            m_udm->lock(value.topic, value.varName, value.varUnit, value.value);
        }
        for (VarBool& value : m_config.outputs.boolVec) {
            m_udm->lock(value.topic, value.varName, value.varUnit, value.value);
        }
        for (VarFloat& value : m_config.outputs.floatVec) {
            m_udm->lock(value.topic, value.varName, value.varUnit, value.value);
        }
        for (VarDouble& value : m_config.outputs.doubleVec) {
            m_udm->lock(value.topic, value.varName, value.varUnit, value.value);
        }
        for (VarString& value : m_config.outputs.stringVec) {
            m_udm->lock(value.topic, value.varName, value.varUnit, value.value);
        }
        for (VarInt64& value : m_config.outputs.int64Vec) {
            m_udm->lock(value.topic, value.varName, value.varUnit, value.value);
        }
    }
    else
    {
        for (VarInt& value : m_config.outputs.intVec) {
            m_udm->unlock(value.topic, value.varName, value.varUnit);
        }
        for (VarBool& value : m_config.outputs.boolVec) {
            m_udm->unlock(value.topic, value.varName, value.varUnit);
        }
        for (VarFloat& value : m_config.outputs.floatVec) {
            m_udm->unlock(value.topic, value.varName, value.varUnit);
        }
        for (VarDouble& value : m_config.outputs.doubleVec) {
            m_udm->unlock(value.topic, value.varName, value.varUnit);
        }
        for (VarString& value : m_config.outputs.stringVec) {
            m_udm->unlock(value.topic, value.varName, value.varUnit);
        }
        for (VarInt64& value : m_config.outputs.int64Vec) {
            m_udm->unlock(value.topic, value.varName, value.varUnit);
        }
    }
    return true;
}



class ElementRequestCb: public RequestCb {
public:
    ElementRequestCb(void* arg, const std::string queueName) : RequestCb(), m_arg(arg),m_queueName(queueName){}
    ~ElementRequestCb()
    {

    }
    void OnRequestRecv(const RequestInfo& topicName, const ByteArray& bytes)override
    {
        Element1* point = (Element1*)m_arg;
        point->onRecvRequest(topicName, m_queueName, bytes);
    }
public:
    void* m_arg;
    std::string m_queueName;
};

class ElementRequestStringCb : public RequestCb {
public:
    ElementRequestStringCb(void* arg, const std::string queueName) : RequestCb(), m_arg(arg), m_queueName(queueName) {}
    ~ElementRequestStringCb()
    {

    }
    void OnRequestRecv(const RequestInfo& topicName, const ByteArray& bytes)override
    {
        Element1* point = (Element1*)m_arg;
        point->onRecvStringRequest(topicName, m_queueName, bytes);
    }
public:
    void* m_arg;
    std::string m_queueName;
};


Element1::Element1(const std::string& name) :
    ElementWithConfig(name)
{
    // Do anything you want done when this object is instantiated here.
    m_name = name;
    logDebug("element " + std::string(__FUNCTION__));
}

Element1::~Element1()
{
    // This is your last chance to clean stuff up.
    logDebug("element " + std::string(__FUNCTION__));
    SAFE_DELETE(m_udm);
    //for (auto it = m_ElementRequestCbList.begin();it != m_ElementRequestCbList.end();)
    //{
    //   
    //    ElementRequestCb* cb = *it;
    //    delete cb;
    //    it = m_ElementRequestCbList.erase(it);
    //}
}

bool Element1::PublishOutputs()
{
    logDebug("element " + std::string(__FUNCTION__));
    for (VarInt &value:m_config.outputs.intVec) {
        m_udm->publish(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarInt64& value : m_config.outputs.int64Vec) {
        m_udm->publish(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarBool &value : m_config.outputs.boolVec) {
        m_udm->publish(value.value, value.topic, value.varName);
    }
    for (VarFloat &value : m_config.outputs.floatVec) {
        m_udm->publish(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarDouble &value : m_config.outputs.doubleVec) {
        m_udm->publish(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarString &value : m_config.outputs.stringVec) {
        m_udm->publish(value.value, value.topic, value.varName);
    }
    return true;
}

bool Element1::SubscribeInputs()
{
    logDebug("element " + std::string(__FUNCTION__));

    for (VarInt &value : m_config.inputs.intVec) {
        m_udm->subscribe(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarInt64& value : m_config.inputs.int64Vec) {
        m_udm->subscribe(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarBool &value : m_config.inputs.boolVec) {
        m_udm->subscribe(value.value, value.topic, value.varName);
    }
    for (VarFloat &value : m_config.inputs.floatVec) {
        m_udm->subscribe(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarDouble &value : m_config.inputs.doubleVec) {
        m_udm->subscribe(value.value, value.topic, value.varName, value.varUnit);
    }
    for (VarString &value : m_config.inputs.stringVec) {
        m_udm->subscribe(value.value, value.topic, value.varName);
    }
    m_udm->subscribe(successedCount, m_name, "status");
    m_udm->subscribe(failedCount, m_name, "failedCount");
    m_udm->subscribe(successedCount, m_name, "successedCount");


    return true;
}

void SEF::Elements::Element1::Element1::SendRequests()
{
    for (int i = 1;i<m_config.requestItems.frequency;i++)
    {
        for (const RequestTopic& requestInfo : m_config.requestItems.publish)
        {
            ByteArray bytes;
            RequestQueryInfo info;

            info.topicName = requestInfo.topic;
            info.queueName = requestInfo.queueName;
            info.requestName = requestInfo.requestName;

            if (requestInfo.isString == false)
            {
                Request request;
                request.setName(requestInfo.requestName);
                request.setSender(m_name);
                request.add("UEstr");
                //request.add(false);
                //request.add(1);
                //request.add(2);
                request.serialize(bytes);
                Request req;
                req.deserialize(bytes);
            }
            else
            {
                bytes.setBytes("test String over Request", 25);
            }


            m_udm->sendRequest(bytes, info);
        }
    }


    //sendElementControl("");
    //sendElementControl(m_name);
}

void SEF::Elements::Element1::Element1::sendElementControl(const SEF_String& name)
{
    ByteArray bytes;
    Request request;
    if (m_count % 2 == 1)
    {
        request.add(static_cast<SEF_Char>(SEF::Kernel::ModeType::Run));
    }
    else
    {
        request.add(static_cast<SEF_Char>(SEF::Kernel::ModeType::Freeze));
    }
    request.add(name);
    request.serialize(bytes);
    Request req;
    RequestQueryInfo queryInfo;
    req.deserialize(bytes);

    queryInfo.topicName = SEF::INTERNAL::TOPIC_SE_CONTROL_REQUEST;
    m_udm->sendRequest(bytes, queryInfo);
}

void Element1::printInput()
{
    if (m_count % logDur != 1)
        return;
    string str= element_name+" check inputs [ runRate+"+to_string(m_runRate) + "fps] :";
    for (VarInt &value : m_config.inputs.intVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarInt64& value : m_config.inputs.int64Vec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarBool &value : m_config.inputs.boolVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarFloat &value : m_config.inputs.floatVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarDouble &value : m_config.inputs.doubleVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarString &value : m_config.inputs.stringVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + value.value;
    }

    logDebug("\n=========printInput=======\n" + str + "\n===========================\n");
}

void Element1::printOutput()
{
    if (m_count % logDur != 1)
        return;
    string str = element_name + " check outputs [ runRate+" + to_string(m_runRate) + "fps] :";
    for (VarInt &value : m_config.outputs.intVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarInt64& value : m_config.outputs.int64Vec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarBool &value : m_config.outputs.boolVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarFloat &value : m_config.outputs.floatVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarDouble &value : m_config.outputs.doubleVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + to_string(value.value);
    }
    for (VarString &value : m_config.outputs.stringVec) {
        str += "\ntopic:" + value.topic + "\tvarname:" + value.varName + "\tvarUnit:" + value.varUnit + "\tvalue:" + value.value;
    }

    logDebug("\n=========printOutput=======\n" + str+"\n======"+ statusStr+ "-failed:"+to_string(failedCount)+" successedCount:"+to_string(successedCount)+"================== = \n");
}

bool Element1::Element1::onRecvRequest(const RequestInfo& topicName, const std::string queueName, const ByteArray& bytes)
{
    pushRequest(topicName.topicName, queueName, bytes);

    if (m_reqCount++ % logDur != 1)
        return true;
    m_udm->log(m_name, SEFLogLevel::Debug,"======"+m_name+"===onRecvRequest=========");

    Request req;
    req.deserialize(bytes);
    SEF_Uint32 count = req.parameterCount();
    for (int i = 0; i < count;i++) {
        ValueType vType = req.getParameterType(i);
        switch (vType) {
            case ValueType::VALUE_TYPE_BOOL: {
                bool value = req.getBool(i);
                logDebug("get Request bool:" + to_string(value));
                break;
            }
            case ValueType::VALUE_TYPE_INT16: {
                SEF_Int16 value = req.getInt16(i);
                logDebug("get Request int16:" + to_string(value));
                break;
            }
            case ValueType::VALUE_TYPE_INT32: {
                SEF_Int32 value = req.getInt32(i);
                logDebug("get Request int32:" + to_string(value));
                break;
            }
            case ValueType::VALUE_TYPE_STRING: {
                SEF_String value = req.getString(i);
                logDebug("get Request string:" + value);
                break;
            }
        }
    }
    return false;
}

bool SEF::Elements::Element1::Element1::onRecvStringRequest(const SEF::Udm::RequestInfo& topicName, const std::string queueName, const ByteArray& bytes)
{
    logDebug("recv request string:"+bytes.toString());
    return true;
}

SEF::COMMON::SEF_Bool SEF::Elements::Element1::Element1::registerRequest(const SEF::COMMON::SEF_String& topicName, const SEF::COMMON::SEF_String& queueName, const SEF::COMMON::SEF_String& request)
{
    if (m_udm)
    {
        RequestQueryInfo info;
        info.queueName = queueName;
        info.requestName = request;
        info.topicName = topicName;
        m_udm->registerRequest(info);
    }
    return true;
}

void SEF::Elements::Element1::Element1::OnConsumedVarInfoRecv(std::vector<MessageInfo>& vec)
{
    for (MessageInfo info : vec) {
        logDebug(m_name+": subscribe TopicesQueryCb recv:" + info.topicName + " " + info.variableName);
    }
}

void SEF::Elements::Element1::Element1::OnProductedVarInfoRecv(std::vector<MessageInfo>& vec)
{
    for (MessageInfo info : vec) {
        logDebug(m_name + ":publish TopicesQueryCb recv:" + info.topicName + " " + info.variableName);
    }
}

void SEF::Elements::Element1::Element1::OnRequestItemRecv(std::vector<RequestInfo>& vec)
{
    for (RequestInfo info : vec) {
        logDebug("request TopicesQueryCb recv:" + info.topicName + " " + info.queueName);
    }
}


bool Element1::openConfigureFile() {
    ConvertType type = ConvertType::NONE;
    std::string::size_type lastPos = config_path.find_last_of('.');
    if (lastPos != std::string::npos && lastPos < config_path.length() - 1)
    {
        std::string extension = config_path.substr(lastPos + 1);
        if (extension == "json")
        {
            type = ConvertType::JSON;
        }
        else
        {
            throw "element use unknown configure file type: " + extension;
        }
    }

    ConfigConvert<ElementConfig> conv(type);
    try {
        m_config = conv.configConvertFromFile(config_path);
    }
    catch (std::string errs) {
        throw "Config error:" + errs;
    }
}

void Element1::startUdm(){
    if (m_udm != nullptr) {
        return;
    }

    openConfigureFile();


    for (UdmInfo info : m_config.udmConfig) {
        m_udm = CreateUdmByType(UdmType(info.type));
        m_udm->setEventCb(this);
        m_udm->SetUdmName(element_name);
        for (UdmAddress addr : info.address) {
            m_udm->appendUdmAddress(addr);
        }
        m_udm->init();
        //if (info.type == (int)UdmType::SocketAPI)
        //{
        //    //m_udpUdm = CreateUdmByType(UdmType(info.type));
        //    //m_udpUdm->SetUdmName(element_name);
        //    //for (UdmAddress addr : info.address) {
        //    //    m_udpUdm->appendUdmAddress(addr);
        //    //}
        //    //m_udpUdm->init();
        //}
        //else if (info.type == (int)UdmType::ZeroMQ)
        //{
        //    m_udm = CreateUdmByType(UdmType(info.type));
        //    m_udm->setEventCb(this);
        //    m_udm->SetUdmName(element_name);
        //    for (UdmAddress addr : info.address) {
        //        m_udm->appendUdmAddress(addr);
        //    }
        //    m_udm->init();
        //}
    }
    std::unique_lock<std::mutex> lock(mMutex);
    mCondition.wait(lock);
    if (isUdmConntected == false)
    {
        logDebug(string("udm is offline"));
        return;
    }


    VarQueryInfo index;
    index.topicName = element_name;

    m_udm->subscribe(this, index);
    m_udm->subscribeRequestQuery(this);
    m_udm->subscribeConsumedVarInfo(this);
    m_udm->subscribeProductedVarInfo(this);
    for (RequestTopic requestTopic : m_config.requestItems.subscribe)
    {
        RequestQueryInfo info;
        info.topicName = requestTopic.topic;
        if (requestTopic.isString == false)
        {
            ElementRequestCb* cb = new ElementRequestCb(this, requestTopic.queueName);
            m_udm->recvRequest(info, cb);
            m_elementRequestCbList.push_back(cb);
        }
        else
        {
            ElementRequestStringCb* cb = new ElementRequestStringCb(this, requestTopic.queueName);
            m_udm->recvRequest(info, cb);
            m_elementRequestStringCbList.push_back(cb);
        }


        registerQueue(info.topicName);
        registerQueue(info.topicName, requestTopic.queueName+"1");
        registerQueue(info.topicName, requestTopic.queueName+"2");
        registerQueue(info.topicName, requestTopic.queueName+"3");
        registerQueue(info.topicName, requestTopic.queueName+"4");
        registerQueue(info.topicName, requestTopic.queueName);
        registerQueueHandler(requestTopic.topic, requestTopic.queueName, requestTopic.requestName, (RequestHandler)&Element1::onRequestHandler);

        //registerQueue(info.topicName);
        //registerQueueHandler(requestTopic.topic, requestTopic.requestName, (RequestHandler)&Element1::onRequestHandler);
    }
    //m_udm->unsubscribeNotify(this);


    logDebug("element " + std::string(__FUNCTION__));
}
bool Element1::Startup()
{
    logDebug("element " + std::string(__FUNCTION__));
    startUdm();
    PublishOutputs();
    SubscribeInputs();
    return true;
}

bool Element1::Initialize()
{
    //if (!getModeComplete())
    //{
    //    setModeComplete(true);
    //}

    //m_vdn.read();

    //handleRequestQueues();
    logDebug("element " + std::string(__FUNCTION__));
    //m_udm->read();
    //readConfig();
    //m_udm->commit();
    //// Your processing goes here.
    return true;
    //m_vdn.commit();
}

bool Element1::Freeze()
{
    logDebug("element " + std::string(__FUNCTION__));
    //m_udm->read();
    //readConfig();
    //m_udm->commit();
    //m_vdn.read();
    //handleRequestQueues();

    //// Your processing goes here.
    return true;
    //m_vdn.commit();
}

bool Element1::Run()
{
    //fprintf(stderr, "=========Test======%s===element %s\n", m_name.c_str(), __FUNCTION__);
    SEF_Uint64 runTime = getMillsec();
    if (m_firstRunTime == 0) 
    {
        m_firstRunTime = runTime;
    }
    //if (m_count >= 500)
    //{
    //    if (m_udm)
    //    {
    //        delete m_udm;
    //        m_udm = nullptr;
    //    }
    //    return false;
    //}
    m_count++;
    handleRequestQueues(true);
    m_udm->read();
    printInput();
    readConfig();
    printOutput();
    m_udm->commit();
    SendRequests();
    m_runCount++;

    //if (m_runCount % 6 == 1)
    //{
    //    PublishOutputsLock(true);
    //}
    //else if (m_runCount % 6 == 4)
    //{
    //    PublishOutputsLock(false);
    //}

    //if (runTime != m_firstRunTime)
    //{
    //    m_runRate = 1000* m_runCount /(runTime - m_firstRunTime);
    //}
    //// Your processing goes here.
    return true;
    //m_vdn.commit();
}

bool Element1::Shutdown()
{
    //Csf::Osal::String str;
    //if (getModeComplete())
    //{
    //    return;
    //}
    //handleRequestQueues();

    //// Your processing goes here.
    logDebug("element " + std::string(__FUNCTION__));
    return true;
    //// Log shutdown message
    //m_vdn.log(Csf::Vdn::Log::e_event, "shutting down.");
    //setModeComplete(true);
}

void Element1::readConfig()
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    auto now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration = now.time_since_epoch();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= sec;
    auto millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    char buffer[80] = { 0 };
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", localTime);
    string day(buffer);
    std::srand(static_cast<unsigned int>(std::time(nullptr)+ millseconds.count()));
    int random = std::rand();
    static SEF_Int64 i64 = 9223372036854775807;
    for (VarInt &value : m_config.outputs.intVec) {
        value.value = random++;
    }
    for (VarInt64& value : m_config.outputs.int64Vec) {
        value.value = i64--;
    }
    for (VarBool &value : m_config.outputs.boolVec) {
        value.value = random++ % 2 == 1 ? true : false;
    }
    for (VarFloat &value : m_config.outputs.floatVec) {
        value.value = static_cast<float>(random++) / RAND_MAX;
    }
    for (VarDouble &value : m_config.outputs.doubleVec) {
        value.value = static_cast<double>(random++) / RAND_MAX;
    }
    for (VarString &value : m_config.outputs.stringVec) {
        value.value = day + "." + to_string(millseconds.count());
    }
}

void Element1::logDebug(const string& logger)const
{
    m_udm->log(m_name, SEFLogLevel::Debug, logger);
}

void Element1::logTrace(const string& logger) const
{
    m_udm->log(m_name, SEFLogLevel::Trace, logger);
}
