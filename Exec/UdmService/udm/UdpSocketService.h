#pragma once
#include "UdmService.h"
#include "UdmMessage.h"
#include "SafeQueue.h"
#include <map>
#include <list>
using namespace SEF::Udm;

class UdpServerCtx;
namespace SEF
{
	namespace UdmService
	{
		class UDM_EXPORT_API UdpSocketService : public UdmService
		{
		public:
			UdpSocketService();
			~UdpSocketService();

			bool init(const char* config)override;
			bool init(unsigned int frequency, unsigned int timeout = 10) override;
			bool addAddress(const UdmAddress& info) override;

			bool start()override;
			void stop() override;

		private:
			UdpServerCtx* m_ctx = nullptr;
			std::list<UdmInfo>m_addrConfig;
		};
	}
}