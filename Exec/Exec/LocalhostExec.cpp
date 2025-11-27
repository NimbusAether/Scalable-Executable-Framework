#include "LocalhostExec.h"
#include "Config.h"
#include "SimpleSingletonUdmFactory.h"
#include "ThreadSafetySingletonUdmFactory.h"
#include "SharedMemoryUdmFactory.h"
#include "MessageQueueUdmFactory.h"
#include "SemaphoreUdmFactory.h"
#include "SocketAPIUdmFactory.h"
#include "ZeroMQUdmFactory.h"

using SEF::Exec::LocalhostExec;

LocalhostExec::LocalhostExec() 
{

}

LocalhostExec::~LocalhostExec() 
{

}

Udm* LocalhostExec::createExecUdm(UdmType udmType) 
{
	UdmFactory* udmFactory = nullptr;
	switch (udmType)
	{
	case UdmType::None:
		break;
	case UdmType::SimpleSingleton:
		udmFactory = new SimpleSingletonUdmFactory();
		udm = udmFactory->createUdm();
		break;
	case UdmType::ThreadSafetySingleton:
		udmFactory = new ThreadSafetySingletonUdmFactory();
		udm = udmFactory->createUdm();
		break;
	case UdmType::SharedMemory:
		udmFactory = new SharedMemoryUdmFactory();
		udm = udmFactory->createUdm();
		break;
	case UdmType::MessageQueue:
		udmFactory = new MessageQueueUdmFactory();
		udm = udmFactory->createUdm();
		break;
	case UdmType::Semaphore:
		udmFactory = new SemaphoreUdmFactory();
		udm = udmFactory->createUdm();
		break;
	case UdmType::SocketAPI:
		udmFactory = new SocketAPIUdmFactory();
		udm = udmFactory->createUdm();
		break;
	case UdmType::ZeroMQ:
		udmFactory = new ZeroMQUdmFactory();
		udm = udmFactory->createUdm();
		break;
	default:
		break;
	}
	return udm;
}

Config* LocalhostExec::createExecConfig(ConfigType configType) 
{
	config = new SEF::Config::Config(configType);
	return config;
}
