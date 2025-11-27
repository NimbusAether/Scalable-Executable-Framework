#include "UdmFactoryGenerator.h"
#include "ZeroMQUdmFactory.h"
#include "UdpSocketUdmFactory.h"
#include "config.h"
using SEF::Udm::UdmFactory;
using namespace SEF::Udm;

Udm * CreateUdmByType(UdmType type)
{
	UdmFactory* factory = nullptr;
	Udm* udm = nullptr;
	switch (type) 
	{
	case UdmType::ZeroMQ:{
		factory = new ZeroMQUdmFactory();
		udm = factory->createUdm();
		break;
	}
	case UdmType::SocketAPI: {
		factory = new UdpSocketUdmFactory();
		udm = factory->createUdm();
		break;
	}
	}
	return udm;
}
