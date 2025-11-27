#pragma once
#include "Kernel.h"

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API KernelFactory
		{
		public:
			virtual Kernel createKernel();
		};
	}
}