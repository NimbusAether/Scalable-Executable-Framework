#include "InteractiveKernelFactory.h"
#include "InteractiveKernel.h"

using SEF::Kernel::InteractiveKernelFactory;
using namespace SEF::Kernel;

Kernel InteractiveKernelFactory::createKernel() 
{
	Kernel *kernel = new InteractiveKernel();
	return *kernel;
}
