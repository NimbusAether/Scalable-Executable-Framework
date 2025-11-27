#include "ZeroMQUdmFactory.h"
#include "ZeroMQUdmEx.h"
using SEF::Udm::ZeroMQUdmFactory;
using namespace SEF::Udm;

Udm* ZeroMQUdmFactory::createUdm()
{
	Udm* udm = new ZeroMQUdmEx();
	LOGFMTT("\ncreate udm client(zmq)\n");
	return udm;
}
