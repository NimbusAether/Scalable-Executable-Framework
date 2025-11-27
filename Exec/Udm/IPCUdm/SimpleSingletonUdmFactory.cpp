#include "SimpleSingletonUdmFactory.h"
#include "SimpleSingletonUdm.h"

using SEF::Udm::SimpleSingletonUdmFactory;
using namespace SEF::Udm;

Udm* SimpleSingletonUdmFactory::createUdm() 
{
	Udm* udm = new SimpleSingletonUdm();
	return udm;
}
