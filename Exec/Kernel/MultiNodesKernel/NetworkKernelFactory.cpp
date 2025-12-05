#include "NetworkKernelFactory.h"
#include "NetworkKernel.h"

using SEF::Kernel::NetworkKernelFactory;
using namespace SEF::Kernel;

Kernel NetworkKernelFactory::createKernel() 
{
	Kernel *kernel = new NetworkKernel();
	return *kernel;
}
