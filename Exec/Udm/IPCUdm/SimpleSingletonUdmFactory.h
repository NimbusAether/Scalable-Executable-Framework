#pragma once
#include "UdmFactory.h"

namespace SEF
{
	namespace Udm
	{
		class UDM_EXPORT_API SimpleSingletonUdmFactory : public UdmFactory
		{
		public:
			Udm* createUdm() override;
		};
	}
}