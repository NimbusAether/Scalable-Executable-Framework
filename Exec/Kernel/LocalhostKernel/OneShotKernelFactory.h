#pragma once
#include "KernelFactory.h"

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API OneShotKernelFactory : public KernelFactory
		{
		public:
			Kernel createKernel();
		};
	}
}
