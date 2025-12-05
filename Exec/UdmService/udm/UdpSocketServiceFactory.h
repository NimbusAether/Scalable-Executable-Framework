#pragma once
#include "UdmServiceFactory.h"

namespace SEF
{
	namespace UdmService
	{
		class UDM_EXPORT_API UdpSocketServiceFactory : public UdmServiceFactory
		{
		public:
			UdmService* createUdmService() override;
		};
	}
}