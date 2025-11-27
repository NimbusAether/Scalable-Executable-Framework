#pragma once
#include "UdmFactory.h"

namespace SEF
{
	namespace Udm
	{
		class UDM_EXPORT_API SocketAPIUdmFactory : public UdmFactory
		{
		public:
			Udm* createUdm() override;
		};
	}
}