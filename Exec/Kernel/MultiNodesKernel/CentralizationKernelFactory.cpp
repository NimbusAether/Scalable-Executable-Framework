#include "CentralizationKernelFactory.h"

using SEF::Kernel::CentralizationKernelFactory;
using namespace SEF::Kernel;

Kernel CentralizationKernelFactory::createKernel() 
{
	Kernel *kernel = new CentralizationKernel();
	return *kernel;
}
