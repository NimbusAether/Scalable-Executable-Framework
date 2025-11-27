#include "OneShotKernelFactory.h"
#include "OneShotKernel.h"

using SEF::Kernel::OneShotKernelFactory;
using namespace SEF::Kernel;

Kernel OneShotKernelFactory::createKernel() 
{
	Kernel *kernel = new OneShotKernel();
	return *kernel;
}
