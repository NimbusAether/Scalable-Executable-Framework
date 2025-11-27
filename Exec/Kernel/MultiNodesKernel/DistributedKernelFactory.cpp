#include "DistributedKernelFactory.h"
#include "DistributedKernel.h"

using SEF::Kernel::DistributedKernelFactory;
using namespace SEF::Kernel;

Kernel DistributedKernelFactory::createKernel() 
{
	Kernel *kernel = new DistributedKernel();
	return *kernel;
}
