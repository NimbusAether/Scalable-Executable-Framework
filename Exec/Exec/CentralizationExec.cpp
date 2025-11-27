#include "CentralizationExec.h"
#include "CentralizationKernelFactory.h"
#include "ElementFactory.h"
#include <list>
#include "OpenLibrary.h"
#include "log.h"
#include "internal.h"
#include "UdmRequestQueue.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using SEF::Exec::CentralizationExec;
using SEF::Element::Element;
using SEF::Kernel::CentralizationKernel;
using SEF::Exec::OpenLibrary;
using SEF::COMMON::Request;
using SEF::COMMON::Parameter;

class RequestControler:public RequestCb {
public:
	RequestControler(CentralizationKernel *kernel):m_kernel(kernel)
	{

	}
	~RequestControler()
	{

	}
	void OnRequestRecv(const RequestInfo& index, const ByteArray& bytes)
	{
		Request request;
		request.deserialize(bytes);
		
		int paramCount = request.parameterCount();
		if (paramCount <= 0)
		{
			LOGFMTE(" CentralizationExec recv request in order to control  elements,have no parameter");
			return;
		}
		if (request.getParameterType(0) != ValueType::VALUE_TYPE_CHAR)
		{
			LOGFMTE(" CentralizationExec recv request in order to control  elements, first parameter type illage:%d", request.getParameterType(0));
			return;
		}
		SEF_Char v = request.getChar(0);
		ModeType modeType = static_cast<ModeType>(v);
		if (paramCount == 1)
		{
			m_kernel->ElementStatControl("", modeType);
		}
		else
		{
			int i = 1;
			for (;i< paramCount;i++)
			{
				if (request.getParameterType(i) == ValueType::VALUE_TYPE_STRING)
				{
					SEF_String elementName = request.getString(i);
					bool res = m_kernel->ElementStatControl(elementName, modeType);
					LOGFMTD("!!!!!!element control: set elementName '%s' to mode %s %s", elementName.c_str(), SEF::Kernel::modeType2String(modeType), res==true?"success":"failed");
				}
			}
		}
	}
private:
	CentralizationKernel* m_kernel = nullptr;
};

inline static int s_stoi(string str)
{
	if (str.empty()) 
	{
		return -1;
	}
	else 
	{
		return stoi(str);
	}
}



CentralizationExec::CentralizationExec() 
{
	kernel = nullptr;
}

CentralizationExec::~CentralizationExec() 
{
	execUdmFree();
	if (m_centralizationKernel != nullptr) 
	{
		delete m_centralizationKernel;
		m_centralizationKernel = nullptr;
	}
}

Kernel* CentralizationExec::createExecKernel() 
{
	m_centralizationKernel = new CentralizationKernel();
	kernel = m_centralizationKernel;
	m_centralizationKernel->setElemeltPublisherCb(this);
	return kernel;
}

vector<Element*> CentralizationExec::createExecElementsByConfig(const string& configString) 
{
	config->openConfigureFile(configString);
	const LogConfig &logConfig = config->GetLogConfig();
	const SEFConfig &gconfig = config->GetSEFConfig();
	vector<GroupInfo> groups = config->GetGroupInfo();

	UdmLogConfig udmLogConfig;
	switch (logConfig.level) 
	{
		case 0: {
			udmLogConfig.m_level = SEFLogLevel::Trace;
			break;
		}
		case 1: {
			udmLogConfig.m_level = SEFLogLevel::Debug;
			break;
		}
		case 2: {
			udmLogConfig.m_level = SEFLogLevel::Info;
			break;
		}
		case 3: {
			udmLogConfig.m_level = SEFLogLevel::Warning;
			break;
		}
		case 4: {
			udmLogConfig.m_level = SEFLogLevel::Error;
			break;
		}
		case 5: {
			udmLogConfig.m_level = SEFLogLevel::Alarm;
			break;
		}
		case 6: {
			udmLogConfig.m_level = SEFLogLevel::Fatal;
			break;
		}
		default:
		{
			udmLogConfig.m_level = SEFLogLevel::Warning;
			break;
		}
	}
	udmLogConfig.m_path = logConfig.path+"\/exec.log";
	UdmLogFactory::setConfig(LogLibType::CLOGGER, &udmLogConfig);

	if (groups.size() > 0)
	{
		for (GroupInfo group: groups) 
		{
			Group* tGroup = new Group();
			tGroup->SetGroupName(group.name);
			for (ElementInfo elemInfo: group.elements)
			{
				if (elemInfo.dllFile.empty() == true) 
				{
					LOGFMTE("open %s element dll failed, no dll path\n", elemInfo.name.c_str());
					continue;
				}
				ElementFactory *elementFactory = dllFactoryOpen(elemInfo.dllFile);
				if (elementFactory == nullptr)
				{
					LOGFMTE("open %s element dll %s failed\n", elemInfo.name.c_str(), elemInfo.dllFile.c_str());
					continue;
				}
				LOGFMTT("start element %s \n", elemInfo.name.c_str());
				if (elemInfo.configFile.empty()==true)
				{
					Element::Element* element = elementFactory->createElement(elemInfo.type, elemInfo.name);
					if (element != nullptr) 
					{
						element->SetElementConfig(group.frequency, true, 0, elemInfo.type);
						tGroup->AddElementToGroup(element);
						elements.push_back(element);
						elementStat stat;
						stat.elementName = element->GetElementName();
						m_statMap.insert(std::make_pair(stat.elementName, stat));
						LOGFMTT("add element %s success\n", elemInfo.name.c_str());
					}
					else 
					{
						LOGFMTT("add element %s failed\n", elemInfo.name.c_str());
					}
				}
				else 
				{
					ElementWithConfig* element = elementFactory->createElementWithConfig(elemInfo.type, elemInfo.name, elemInfo.configFile);
					if (element != nullptr) 
					{
						element->SetElementConfig(group.frequency, true, 0, elemInfo.type, elemInfo.configFile);
						tGroup->AddElementToGroup(element);
						elements.push_back(element);
						elementStat stat;
						stat.elementName = element->GetElementName();
						m_statMap.insert(std::make_pair(stat.elementName, stat));
						fprintf(stderr,"add element %s success\n", elemInfo.name.c_str());
					}
					else 
					{
						LOGFMTT("add element %s failed\n", elemInfo.name.c_str());
					}
				}
			}
			m_centralizationKernel->AddElementsGroup(tGroup);
			m_centralizationKernel->SetMultiNodesKernelConfig(gconfig.frequency, 0, gconfig.startupTimeout,
				gconfig.initializeTimeout, gconfig.shutdownTimeout);
		}
	}
	
	/*
	* 根据configure 初始化UDM服务
	*/
	std::vector<UdmInfo> list = config->GetUdmServiceConfig();
	bool ret = false;
	for (UdmInfo udm : list)
	{
		UdmService::UdmService * udmService = UdmServiceCreate(UdmType(udm.type));
		if (udmService == nullptr)
		{
			LOGFMTE("start udm service %d failed\n", udm.type);
			_sleep(1000);
			exit(0);
		}
		ret = udmService->init(100, 100);
		if (ret == false)
		{
			LOGFMTE("init udm service %d failed\n", udm.type);
			_sleep(1000);
			exit(0);
		}
		for (UdmAddress address : udm.address)
		{
			ret = udmService->addAddress(address);
			if (ret == false)
			{
				LOGFMTE("udm service %d set address[%s:%d] failed\n", udm.type, address.ip.c_str(), address.port);
				_sleep(1000);
				exit(0);
			}
		}
		udmService->start();
	}

	/*
	* 根据configure 初始化UDM客户端，实现request控制
	*/
	const UdmInfo & info = config->GetUdmClientConfig();
	if (info.address.size()>0)
	{
		m_udm = createExecUdm(UdmType(info.type));
		m_udm->SetUdmName("UdmClient");
		for (UdmAddress addr : info.address) {
			m_udm->appendUdmAddress(addr);
		}
		m_udm->init();
		m_udm->setEventCb(this);
	}


	return elements;
}

void CentralizationExec::RunExec() 
{

	/*
	* run kernelExec
	*/
	m_centralizationKernel->ExecuteKernel();
	//do {
	//	kernel->ExecuteKernel();
	//	for (Element::Element* element : elements) {
	//		element->Run();
	//	}

	//	_sleep(1000);
	//} while (true);
}


ElementFactory* CentralizationExec::dllFactoryOpen(string& dllpath)
{
	OpenLibrary dllOpen;
	ElementFactory * factory = dllOpen.LoaderFactory(dllpath.c_str());
	return factory;
}

void SEF::Exec::CentralizationExec::countPublisher(std::vector<elementStat>& vec)
{
	/*修改变量值*/
	for (elementStat stat:vec)
	{
		auto it = m_statMap.find(stat.elementName);
		if (it != m_statMap.end())
		{
			elementStat& entry = it->second;
			entry.failedCount = stat.failedCount;
			entry.status = stat.status;
			entry.successedCount = stat.successedCount;
			LOGFMTD("element Status publish:%s  successed:%d; failed:%d", entry.elementName.c_str(), entry.successedCount, entry.failedCount);
		}
	}

	m_udm->commit();
}

void SEF::Exec::CentralizationExec::OnEventRecv(const UdmEventType& type)
{
	if (type == UdmEventType::UDM_EVENT_CONNECTED&& m_publishSet == false)
	{
		m_publishSet = true;
		RequestControler* cb = new RequestControler(m_centralizationKernel);
		RequestQueryInfo queryInfo;
		queryInfo.topicName = SEF::INTERNAL::TOPIC_SE_CONTROL_REQUEST;
		m_udm->recvRequest(queryInfo, cb);

		for (auto & it: m_statMap)
		{
			elementStat& stat = it.second;
			m_udm->publish(stat.status, stat.elementName, "status");
			m_udm->publish(stat.failedCount, stat.elementName, "failedCount");
			m_udm->publish(stat.successedCount, stat.elementName, "successedCount");
		}
	}

}
