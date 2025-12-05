#pragma once
#include "UdmFactory.h"

namespace SEF
{
	namespace Udm
	{
		class UDM_EXPORT_API SharedMemoryUdmFactory : public UdmFactory
		{
		public:
			Udm* createUdm() override;
		};
	}
}
