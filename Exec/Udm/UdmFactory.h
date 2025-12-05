#pragma once
#include "Udm.h"
#include "base.h"
namespace SEF
{
	namespace Udm
	{
		class UDM_EXPORT_API UdmFactory
		{
		public:
			virtual Udm* createUdm() = 0;
		};
	}
}