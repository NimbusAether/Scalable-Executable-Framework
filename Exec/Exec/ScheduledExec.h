#pragma once
#include "LocalhostExec.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API ScheduledExec : public LocalhostExec
		{
		public:
			ScheduledExec();
			~ScheduledExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(string configString);
			void RunExec() override;
		};
	}
}