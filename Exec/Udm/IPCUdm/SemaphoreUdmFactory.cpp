#include "SemaphoreUdmFactory.h"
#include "SemaphoreUdm.h"

using SEF::Udm::SemaphoreUdmFactory;
using namespace SEF::Udm;

Udm* SemaphoreUdmFactory::createUdm() 
{
	Udm* udm = new SemaphoreUdm();
	return udm;
}
