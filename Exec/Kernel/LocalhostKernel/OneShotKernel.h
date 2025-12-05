#pragma once
#include "LocalhostKernel.h"

namespace SEF
{
	namespace Kernel
	{
		class OneShotKernel : public LocalhostKernel
		{
		public:
			OneShotKernel();
			~OneShotKernel();
		protected:
		};
	}
}