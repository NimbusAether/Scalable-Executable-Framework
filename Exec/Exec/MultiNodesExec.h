#pragma once
#include <thread>
#include <list>
#include "Exec.h"
#include "Element.h"
#include "Config.h"
#include "Udm.h"
#include "UdmService.h"
using namespace SEF::UdmService;
using namespace SEF::Element;
using namespace SEF::Config;
using namespace SEF::Udm;

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API MultiNodesExec : public Exec
		{
		public:
			MultiNodesExec();
			virtual ~MultiNodesExec();
			Config::Config* createExecConfig(ConfigType configType);
			virtual vector<Element::Element*> createExecElementsByConfig(const string& configString) = 0;
			UdmService::UdmService* UdmServiceCreate(UdmType udmType);
		protected:
			Udm::Udm* m_udm = nullptr;
			Config::Config* config = nullptr;
			vector<Element::Element*> elements;
			std::list<UdmService::UdmService*> m_udmServiceList;
			Udm::Udm* createExecUdm(UdmType udmType);
			void execUdmFree();
			//UdmService::UdmService* m_udmService = nullptr;
		};
	}
}
