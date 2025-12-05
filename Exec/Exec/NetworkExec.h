#pragma once
#include "MultiNodesExec.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API NetworkExec : public MultiNodesExec
		{
		public:
			NetworkExec();
			~NetworkExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(const string& configString) override;
			void RunExec() override;
		};
	}
}
