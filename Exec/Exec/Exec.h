#pragma once
#include "Kernel.h"

using namespace SEF::Kernel;

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API Exec
		{
		public:
			Exec();
			virtual ~Exec();
			virtual Kernel::Kernel* createExecKernel() = 0;
			virtual void RunExec() = 0;
		protected:
			Kernel::Kernel* kernel;
		};
	}
}