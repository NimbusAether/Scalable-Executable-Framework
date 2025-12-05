#pragma once
#include "KernelFactory.h"
#include "CentralizationKernel.h"

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API CentralizationKernelFactory : public KernelFactory
		{
		public:
			Kernel createKernel();
		};
	}
}