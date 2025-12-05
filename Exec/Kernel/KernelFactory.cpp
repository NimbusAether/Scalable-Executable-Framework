#include "KernelFactory.h"

using SEF::Kernel::KernelFactory;
using namespace SEF::Kernel;

Kernel KernelFactory::createKernel() 
{
	Kernel *kernel = new Kernel();
	return *kernel;
}
