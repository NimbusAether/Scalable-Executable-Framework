#pragma once
#include "LocalhostExec.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API InteractiveExec : public LocalhostExec
		{
		public:
			InteractiveExec();
			~InteractiveExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(string configString);
			void RunExec() override;
		};
	}
}
