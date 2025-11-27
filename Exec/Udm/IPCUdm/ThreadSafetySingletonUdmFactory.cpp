#include "ThreadSafetySingletonUdmFactory.h"
#include "ThreadSafetySingletonUdm.h"

using SEF::Udm::ThreadSafetySingletonUdmFactory;
using namespace SEF::Udm;

Udm* ThreadSafetySingletonUdmFactory::createUdm() 
{
	Udm* udm = new ThreadSafetySingletonUdm();
	return udm;
}
