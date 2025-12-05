#include "UdpSocketUdmFactory.h"
#include "UdpSocketUdm.h"
using SEF::Udm::UdpSocketUdmFactory;
using namespace SEF::Udm;

Udm* UdpSocketUdmFactory::createUdm()
{
	Udm* udm = new UdpSocketUdm();
	LOGFMTT("create udm client\n");
	return udm;
}
