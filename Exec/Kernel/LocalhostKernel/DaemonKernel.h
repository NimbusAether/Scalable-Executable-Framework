#pragma once
#include "LocalhostKernel.h"

namespace SEF
{
	namespace Kernel
	{
		class DaemonKernel : public LocalhostKernel
		{
		public:
			DaemonKernel();
			~DaemonKernel();
		protected:

		};
	}
}