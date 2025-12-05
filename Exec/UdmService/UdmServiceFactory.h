#pragma once
#include "UdmService.h"

namespace SEF
{
	namespace UdmService
	{
		class UDM_EXPORT_API UdmServiceFactory
		{
		public:
			virtual UdmService* createUdmService() = 0;
		};
	}
}