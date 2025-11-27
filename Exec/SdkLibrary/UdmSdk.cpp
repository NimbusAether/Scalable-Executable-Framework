#include "UdmSdk.h"
#include "ZeroMQUdmFactory.h"
#include "config.h"

using SEF::Udm::UdmFactory;
using namespace SEF::Udm;

Udm * CreateUdmByType(int type)
{
	UdmFactory* factory = nullptr;
	Udm* udm = nullptr;
	switch ((UdmType)type) 
	{
	case UdmType::ZeroMQ:{
		factory = new ZeroMQUdmFactory();
		udm = factory->createUdm();
		break;
	}
	case UdmType::FastDDS:
		break;
	}
	return udm;
}


SEF::Request* CreateRequest()
{
	return new SEF::Request();
}

SEF::Parameter* CreateParamter()
{
	return new SEF::Parameter();
}

SEF::COMMON::ByteArray* CreateByteArray()
{
	return new SEF::COMMON::ByteArray();
}
