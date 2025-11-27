#pragma once
#include "LocalhostExec.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API DaemonExec : public LocalhostExec
		{
		public:
			DaemonExec();
			~DaemonExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(string configString);
			void RunExec() override;
		};
	}
}