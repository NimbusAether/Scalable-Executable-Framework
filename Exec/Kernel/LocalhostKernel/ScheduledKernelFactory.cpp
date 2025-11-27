#include "ScheduledKernelFactory.h"
#include "ScheduledKernel.h"

using SEF::Kernel::ScheduledKernelFactory;
using namespace SEF::Kernel;

Kernel ScheduledKernelFactory::createKernel() 
{
	Kernel *kernel = new ScheduledKernel();
	return *kernel;
}
