#include "DaemonKernelFactory.h"
#include "DaemonKernel.h"

using SEF::Kernel::DaemonKernelFactory;
using namespace SEF::Kernel;

Kernel DaemonKernelFactory::createKernel() 
{
	Kernel *kernel = new DaemonKernel();
	return *kernel;
}
