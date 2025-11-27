#pragma once

#include "ElementWithConfig.h"
#include "udm.h"
// STL and external lib headers next
#include <iostream>
#include "ConfigConvert.h"
#include <vector>

using SEF::Udm::Udm;
using SEF::Element::ElementWithConfig;
using SEF::Udm::MessageInfo;
using SEF::Udm::RequestInfo;

struct VarInt {
    string  topic;
    string  varName;
    string  varUnit;
    int     value;
};
CONFIG_SERIALIZE(VarInt)
{
    CONFIG_SWAP(topic);
    CONFIG_SWAP(varName);
    CONFIG_SWAP(varUnit);
    CONFIG_SWAP(value);
}
struct VarInt64 {
    string  topic;
    string  varName;
    string  varUnit;
    SEF_Int64     value;
};
CONFIG_SERIALIZE(VarInt64)
{
    CONFIG_SWAP(topic);
    CONFIG_SWAP(varName);
    CONFIG_SWAP(varUnit);
    CONFIG_SWAP(value);
}

struct VarBool {
    string  topic;
    string  varName;
    string  varUnit;
    bool     value;
};
CONFIG_SERIALIZE(VarBool)
{
    CONFIG_SWAP(topic);
    CONFIG_SWAP(varName);
    CONFIG_SWAP(varUnit);
    CONFIG_SWAP(value);
}

struct VarFloat {
    string  topic;
    string  varName;
    string  varUnit;
    float   value;
};
CONFIG_SERIALIZE(VarFloat)
{
    CONFIG_SWAP(topic);
    CONFIG_SWAP(varName);
    CONFIG_SWAP(varUnit);
    CONFIG_SWAP(value);
}

struct VarDouble {
    string  topic;
    string  varName;
    string  varUnit;
    double  value;
};
CONFIG_SERIALIZE(VarDouble)
{
    CONFIG_SWAP(topic);
    CONFIG_SWAP(varName);
    CONFIG_SWAP(varUnit);
    CONFIG_SWAP(value);
}

struct VarString {
    string  topic;
    string  varName;
    string  varUnit;
    string  value;
};
CONFIG_SERIALIZE(VarString)
{
    CONFIG_SWAP(topic);
    CONFIG_SWAP(varName);
    CONFIG_SWAP(varUnit);
    CONFIG_SWAP(value);
}

struct Inputs {
    std::vector<VarInt> intVec;
    std::vector<VarInt64> int64Vec;
    std::vector<VarBool> boolVec;
    std::vector<VarFloat> floatVec;
    std::vector<VarDouble> doubleVec;
    std::vector<VarString> stringVec;
};
CONFIG_SERIALIZE(Inputs)
{
    CONFIG_SWAP(intVec);
    CONFIG_SWAP(int64Vec);
    CONFIG_SWAP(boolVec);
    CONFIG_SWAP(floatVec);
    CONFIG_SWAP(doubleVec);
    CONFIG_SWAP(stringVec);
}

struct Outputs {
    std::vector<VarInt> intVec;
    std::vector<VarInt64> int64Vec;
    std::vector<VarBool> boolVec;
    std::vector<VarFloat> floatVec;
    std::vector<VarDouble> doubleVec;
    std::vector<VarString> stringVec;
};
CONFIG_SERIALIZE(Outputs)
{
    CONFIG_SWAP(intVec);
    CONFIG_SWAP(int64Vec);
    CONFIG_SWAP(boolVec);
    CONFIG_SWAP(floatVec);
    CONFIG_SWAP(doubleVec);
    CONFIG_SWAP(stringVec);
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
    CONFIG_SWAP(configFile);
    CONFIG_SWAP(address);
}

struct RequestTopic {
    std::string topic;
    std::string queueName;
    std::string requestName;
    bool        isString = false;
};
CONFIG_SERIALIZE(RequestTopic)
{
    CONFIG_SWAP(topic);
    CONFIG_SWAP(queueName);
    CONFIG_SWAP(requestName);
    CONFIG_SWAP(isString);
}

struct RequestItems {
    int                       frequency = 1;
    std::vector<RequestTopic> publish;
    std::vector<RequestTopic> subscribe;
};
CONFIG_SERIALIZE(RequestItems)
{
    CONFIG_SWAP(frequency);
    CONFIG_SWAP(publish);
    CONFIG_SWAP(subscribe);
}


struct ElementConfig {
    Outputs outputs;
    Inputs inputs;
    RequestItems requestItems;
    std::vector<UdmInfo> udmConfig;
};
CONFIG_SERIALIZE(ElementConfig)
{
    CONFIG_SWAP(outputs);
    CONFIG_SWAP(inputs);
    CONFIG_SWAP(requestItems);
    CONFIG_SWAP(udmConfig);
}
class ENotifyCb;
class ElementTopicesQueryCb;
class ElementRequestCb;
class ElementRequestStringCb;

namespace SEF
{
        namespace Elements
        {
            namespace Element1
            {

                class Element1 : public ElementWithConfig, public SEF::Udm::RequestItemCb, public SEF::Udm::ConsumedVarInfoCb, public SEF::Udm::ProductedVarInfoCb, public SEF::Udm::NotifyCb,public SEF::Udm::UdmEventCb
                {
                public:

                    /** Ctor
                    * @param name Name of this sim element.
                    */
                    Element1(const std::string& name);

                    /** Dtor. */
                    virtual ~Element1();

                    /** Method called during startup mode. */
                    bool Startup()override;

                    /** Method called during the intialize mode. */
                    bool Initialize()override;

                    /** Method called during the freeze mode. */
                    bool Freeze();

                    /** Method called during the run mode. */
                    bool Run();

                    /** Method called during the shutdown mode. */
                    bool Shutdown();

                    /** Method called during the publish minor mode during the startup mode. */
                    bool PublishOutputs();

                    /** Method called during the subscribe minor mode during the startup mode.
                    * It should be noted that all simulation elements publish their outputs prior
                    * to the subscription minor mode.
                    */
                    bool SubscribeInputs();

                    void SendRequests();

                    void sendElementControl(const SEF_String& name);

                    bool onRecvRequest(const SEF::Udm::RequestInfo& topicName, const std::string queueName, const ByteArray& bytes);

                    bool onRecvStringRequest(const SEF::Udm::RequestInfo& topicName, const std::string queueName, const ByteArray& bytes);

                    SEF::COMMON::SEF_Bool registerRequest(const SEF::COMMON::SEF_String& topicName, const SEF::COMMON::SEF_String& queueName, const SEF::COMMON::SEF_String& request)override;

                    void OnRequestItemRecv(std::vector<RequestInfo>& vec) override;

                    void OnConsumedVarInfoRecv(std::vector<MessageInfo>& vec) override;

                    void OnProductedVarInfoRecv(std::vector<MessageInfo>& vec) override;

                    void OnVarRecv(const MessageInfo& Index, const std::any& value)override;

                    //void onQueueRequestHandler(const RequestQueueArgs&arg);

                    void OnEventRecv(const SEF::Udm::UdmEventType& type)override;


                private:
                    void printInput();
                    void printOutput();
                    bool openConfigureFile();
                    void onRequestHandler(const RequestQueueArgs& arg);
                    bool PublishOutputsLock(bool locked);

                protected:
                    void startUdm();
                    Udm::Udm * m_udm = nullptr;
                    Udm::Udm* m_udpUdm = nullptr;

                    /** VDN inputs for the Template class. */


                    /** VDN outputs for the Template class. */
     

                    /** VDN locals for the Template class. */
                    ElementConfig m_config;

                    class TesTRequest
                    {
                    public:
                        string buf;
                    };

                    /** Function for reading the standard XML Sim Element configuration file. */
                    void readConfig();

                    /** Inputs that this SE subscribes to from the VDN. */


                    /** Outputs this SE publishes to the VDN. */
   

                    /** Local variables this SE publishes on the VDN. */

                    std::list<ElementRequestCb*> m_elementRequestCbList;
                    std::list<ElementRequestStringCb*> m_elementRequestStringCbList;

                    TesTRequest m_reuqest_pub;
                    string m_name;
                    int m_count = 0;
                    int m_reqCount = 0;
                    SEF_Uint64 m_firstRunTime = 0;
                    SEF_Uint64 m_runCount = 0;
                    SEF_Uint64 m_runRate = 0;
                private:
                    mutable std::mutex mMutex;
                    std::condition_variable mCondition;
                    bool isUdmConntected = false;

                    SEF_Int successedCount = 0;
                    SEF_Int failedCount = 0;
                    SEF_String statusStr;
                public:
                    void logDebug(const string& logger)const;
                    void logTrace(const string& logger)const;
                };
            }
        }

}
