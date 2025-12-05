#include "ZeroMQServiceFactory.h"
#include "ZeroMQServiceEx.h"

using namespace SEF::UdmService;


UdmService* ZeroMQServiceFactory::createUdmService()
{
	UdmService* udmService = new ZeroMQServiceEx();
	return udmService;
}
