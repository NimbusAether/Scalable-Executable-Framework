#pragma once
#include "UdmService.h"
#include "zmq.h"
#include "UdmMessage.h"
#include "SafeQueue.h"
#include <map>
#include <list>
using namespace SEF::Udm;

class ZeroServerCtxEx;
namespace SEF
{
	namespace UdmService
	{
		class ZeroMQServiceEx : public UdmService
		{
		public:
			ZeroMQServiceEx();
			~ZeroMQServiceEx();

			bool init(const char* config)override;
			bool init(unsigned int frequency, unsigned int timeout = 10) override;
			bool addAddress(const UdmAddress&info) override;

			bool start()override;
			void stop() override;

		private:
			bool ZeroInit();
			ZeroServerCtxEx* m_ctx = nullptr;
			std::list<UdmInfo>m_addrConfig;
		};
	}
}