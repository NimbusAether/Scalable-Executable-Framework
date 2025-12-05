#include "UdpSocketServiceFactory.h"
#include "UdpSocketService.h"

using namespace SEF::UdmService;


UdmService* UdpSocketServiceFactory::createUdmService()
{
	UdmService* udmService = new UdpSocketService();
	return udmService;
}
