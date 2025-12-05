#pragma once
#include "KernelFactory.h"

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API DaemonKernelFactory : public KernelFactory
		{
		public:
			Kernel createKernel();
		};
	}
}

