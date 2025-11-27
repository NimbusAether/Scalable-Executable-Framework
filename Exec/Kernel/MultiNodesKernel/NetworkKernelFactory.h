#pragma once
#include "KernelFactory.h"

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API NetworkKernelFactory : public KernelFactory
		{
		public:
			Kernel createKernel();
		};
	}
}
