#include "SharedMemoryUdmFactory.h"
#include "SharedMemoryUdm.h"

using SEF::Udm::SharedMemoryUdmFactory;
using namespace SEF::Udm;

Udm* SharedMemoryUdmFactory::createUdm() 
{
	Udm* udm = new SharedMemoryUdm();
	return udm;
}
