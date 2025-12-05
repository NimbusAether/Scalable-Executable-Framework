#include "SocketAPIUdmFactory.h"
#include "SocketAPIUdm.h"

using SEF::Udm::SocketAPIUdmFactory;
using namespace SEF::Udm;

Udm* SocketAPIUdmFactory::createUdm()
{
	Udm* udm = new SocketAPIUdm();
	return udm;
}
