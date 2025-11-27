#pragma once
#include "LocalhostExec.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API BatchExec : public LocalhostExec
		{
		public:
			BatchExec();
			~BatchExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(string configString);
			void RunExec() override;
		};
	}
}
