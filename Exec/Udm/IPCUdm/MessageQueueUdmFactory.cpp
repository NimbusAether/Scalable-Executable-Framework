#include "MessageQueueUdmFactory.h"
#include "MessageQueueUdm.h"

using SEF::Udm::MessageQueueUdmFactory;
using namespace SEF::Udm;

Udm* MessageQueueUdmFactory::createUdm() 
{
	Udm* udm = new MessageQueueUdm();
	return udm;
}
