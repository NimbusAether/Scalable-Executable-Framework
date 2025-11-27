#pragma once
#include "KernelFactory.h"

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API BatchKernelFactory : public KernelFactory
		{
		public:
			Kernel createKernel();
		};
	}
}
