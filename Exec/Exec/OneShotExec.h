#pragma once
#include "LocalhostExec.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API OneShotExec : public LocalhostExec
		{
		public:
			OneShotExec();
			~OneShotExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(string configString);
			void RunExec() override;
		};
	}
}
