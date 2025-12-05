#include "MultiNodesExec.h"
#include "Config.h"
#include "SocketAPIUdmFactory.h"
#include "ZeroMQUdmFactory.h"
#include "ZeroMQServiceFactory.h"
#include "UdpSocketServiceFactory.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using namespace SEF::UdmService;

using SEF::Exec::MultiNodesExec;

MultiNodesExec::MultiNodesExec() {

}

MultiNodesExec::~MultiNodesExec() {
	for (Element::Element*elem: elements)
	{
		delete elem;
	}
	elements.clear();
	if (config)
	{
		delete config;
	}
	if (m_udm)
	{
		delete m_udm;
		m_udm = nullptr;
	}
	for (UdmService::UdmService*ser: m_udmServiceList)
	{
		delete ser;
	}

}

UdmService * MultiNodesExec::UdmServiceCreate(UdmType udmType)
{
	UdmService::UdmService* service = nullptr;
	switch (udmType)
	{
	case UdmType::None:
		break;
	case UdmType::SocketAPI:
	{
		UdmServiceFactory* udmServiceFactory = new UdpSocketServiceFactory();
		service = udmServiceFactory->createUdmService();
		m_udmServiceList.push_back(service);
		break;
	}
	case UdmType::ZeroMQ:
	{
		UdmServiceFactory* udmServiceFactory = new ZeroMQServiceFactory();
		service = udmServiceFactory->createUdmService();
		m_udmServiceList.push_back(service);
		break;
	}
	case UdmType::FastDDS:

		break;
	default:
		break;
	}
	return service;
}

Udm* MultiNodesExec::createExecUdm(UdmType udmType) 
{
	UdmFactory* udmFactory = nullptr;
	switch (udmType)
	{
	case UdmType::None:
		break;
	case UdmType::SocketAPI:
		udmFactory = new SocketAPIUdmFactory();
		m_udm = udmFactory->createUdm();
		break;
	case UdmType::ZeroMQ:
		udmFactory = new ZeroMQUdmFactory();
		m_udm = udmFactory->createUdm();
		break;
	default:
		break;
	}
	return m_udm;
}

void SEF::Exec::MultiNodesExec::execUdmFree()
{
	if (m_udm)
	{
		delete m_udm;
		m_udm = nullptr;
	}
}

Config* MultiNodesExec::createExecConfig(ConfigType configType) 
{
	config = new SEF::Config::Config(configType);
	return config;
}
