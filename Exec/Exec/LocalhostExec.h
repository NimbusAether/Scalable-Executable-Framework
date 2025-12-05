#pragma once
#include "Exec.h"
#include "Element.h"
#include "Config.h"
#include "Udm.h"

using namespace SEF::Element;
using namespace SEF::Config;
using namespace SEF::Udm;

namespace SEF {
	namespace Exec {
		class UDM_EXPORT_API LocalhostExec : public Exec
		{
		public:
			LocalhostExec();
			~LocalhostExec();
			Udm::Udm* createExecUdm(UdmType udmType);
			Config::Config* createExecConfig(ConfigType configType);
			virtual vector<Element::Element*> createExecElementsByConfig() = 0;
		protected:
			Udm::Udm* udm = nullptr;
			Config::Config* config = nullptr;
			vector<Element::Element*> elements;
		};
	}
}