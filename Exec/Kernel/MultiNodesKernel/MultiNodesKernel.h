#pragma once
#include "Kernel.h"
#include "Udm.h"

using namespace SEF::Udm;

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API MultiNodesKernel : public Kernel
		{
		public:
			MultiNodesKernel();
			virtual ~MultiNodesKernel();
			void SetMultiNodesKernelConfig(int frequency, int port, int startup_timeout, int initialize_timeout, int shutdown_timeout);
			int GetKernelFrame() const;
			int GetUdmPort() const;
			UdmType GetUdmType() const;
			string GetUdmUrl()const;
			void SetUdmConfig(UdmType udmType, int udmIp);
			/*
			* SetUdmUrl 兼容非网络消息队列url
			*/
			void SetUdmUrl(UdmType udmType, string& url);
			virtual void ExecuteKernel();
		protected:
			bool Process();
			bool Startup();
			bool PublishOutputs();
			bool SubscribeInputs();
			bool Initialize();
			bool Freeze();
			bool Run();
			bool Shutdown();
		protected:
			int frequency;
			int startup_timeout;
			int initialize_timeout;
			int run_timeout;
			int shutdown_timeout;
			int kernel_frame;
			int udm_port;
			UdmType udm_type;
			string udm_url;
		};
	}
}