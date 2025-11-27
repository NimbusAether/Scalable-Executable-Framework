#pragma once
#include "MultiNodesExec.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API DistributedExec : public MultiNodesExec
		{
		public:
			DistributedExec();
			~DistributedExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(const string& configString) override;
			void RunExec() override;
		};
	}
}
