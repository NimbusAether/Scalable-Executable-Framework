#include "BatchKernelFactory.h"
#include "BatchKernel.h"

using SEF::Kernel::BatchKernelFactory;
using namespace SEF::Kernel;

Kernel BatchKernelFactory::createKernel() 
{
	Kernel *kernel = new BatchKernel();
	return *kernel;
}
