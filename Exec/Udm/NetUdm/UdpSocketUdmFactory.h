#pragma once
#include "UdmFactory.h"

namespace SEF
{
	namespace Udm
	{
		class UDM_UDM_API UdpSocketUdmFactory : public UdmFactory
		{
		public:
			Udm* createUdm() override;
		};
	}
}
