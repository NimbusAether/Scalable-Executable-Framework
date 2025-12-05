#pragma once
#include "MultiNodesExec.h"
#include "CentralizationKernel.h"
#include "ElementFactory.h"
#include "udm.h"

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API CentralizationExec : public MultiNodesExec, public SEF::Kernel::elementStatusPublisher, public SEF::Udm::UdmEventCb
		{
		public:
			CentralizationExec();
			~CentralizationExec();
			Kernel::Kernel* createExecKernel() override;
			vector<Element::Element*> createExecElementsByConfig(const string& configString) override;
			void RunExec() override;

		private:
			ElementFactory* dllFactoryOpen(string& dllpath);
			void countPublisher(std::vector<elementStat>& vec) override;
			void OnEventRecv(const UdmEventType& type) override;
		private:
			CentralizationKernel* m_centralizationKernel = nullptr;
			std::unordered_map<SEF_String, elementStat> m_statMap;
			bool m_publishSet = false;
		};
	}
}