#include "MultiNodesKernel.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using SEF::Kernel::MultiNodesKernel;

MultiNodesKernel::MultiNodesKernel()
{
	frequency = 0;
	startup_timeout = 0;
	initialize_timeout = 0;
	run_timeout = 0;
	shutdown_timeout = 0;
	kernel_frame = 0;
	udm_port = 0;
	udm_type = UdmType::None;
}

MultiNodesKernel::~MultiNodesKernel() 
{

}

void MultiNodesKernel::SetMultiNodesKernelConfig(int frequency, int port, int startup_timeout, int initialize_timeout, int shutdown_timeout) 
{
	MultiNodesKernel::frequency = frequency;
	MultiNodesKernel::udm_port = port;
	MultiNodesKernel::startup_timeout = startup_timeout;
	MultiNodesKernel::initialize_timeout = initialize_timeout;
	MultiNodesKernel::shutdown_timeout = shutdown_timeout;
	MultiNodesKernel::run_timeout = 1000 / frequency;
}

int MultiNodesKernel::GetKernelFrame() const
{
	return kernel_frame;
}

int MultiNodesKernel::GetUdmPort() const 
{
	return udm_port;
}

UdmType MultiNodesKernel::GetUdmType() const 
{
	return udm_type;
}

string MultiNodesKernel::GetUdmUrl() const
{
	return udm_url;
}

void MultiNodesKernel::SetUdmConfig(UdmType udmType, int udmPort) 
{
	udm_type = udmType;
	udm_port = udmPort;
}

void MultiNodesKernel::SetUdmUrl(UdmType udmType, string& url)
{
	udm_type = udmType;
	udm_url = url;
}

bool MultiNodesKernel::Process() 
{
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Process Mode." << endl;
		kernel_mode = ModeType::Process;
		mode_changed = true;
		next_mode = ModeType::Startup;
		return true;
	}
	catch (exception) {
		return false;
	}
	
}

bool MultiNodesKernel::Startup()
{
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Startup Mode." << endl;
		kernel_mode = ModeType::Process;
		mode_changed = true;
		if (!PublishOutputs())
		{
			return false;
			Exit();
		}
		if (!SubscribeInputs())
		{
			return false;
			Exit();
		}
		next_mode = ModeType::Initialize;
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool MultiNodesKernel::PublishOutputs() {
	try {
		cout << kernel_name << "|Publish Outputs success." << endl;
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool MultiNodesKernel::SubscribeInputs() {
	try {
		cout << kernel_name << "|Subscribe Inputs success." << endl;
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool MultiNodesKernel::Initialize() {
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Initialize Mode." << endl;
		kernel_mode = ModeType::Initialize;
		mode_changed = true;
		next_mode = ModeType::Freeze;
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool MultiNodesKernel::Freeze() {
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Freeze Mode." << endl;
		kernel_mode = ModeType::Freeze;
		mode_changed = true;
		next_mode = ModeType::Run;
		return true;
	}
	catch (exception) {
		return false;
	}
	
}

bool MultiNodesKernel::Run() {
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Run Mode." << endl;
		kernel_mode = ModeType::Run;
		if (kernel_frame < frequency)
		{
			kernel_frame++;
		}
		else
		{
			kernel_frame = 0;
		}
		mode_changed = true;
		next_mode = ModeType::Shutdown;
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool MultiNodesKernel::Shutdown() {
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Shutdown Mode." << endl;
		kernel_mode = ModeType::Shutdown;
		mode_changed = true;
		next_mode = ModeType::Exit;
		return true;
	}
	catch (exception) {
		return false;
	}
}

void MultiNodesKernel::ExecuteKernel() {
	LOGFMTT("Begin MultiNodesKernel ExecuteKernel name %s kernel_mode %d\n", kernel_name.c_str(), kernel_mode);

	if (kernel_type != KernelType::None && kernel_name != "")
	{
		if (mode_changed && next_mode == ModeType::Idle)
		{
			if (!Idle())
			{
				Exit();
			}
		}

		if (mode_changed && next_mode == ModeType::Process)
		{
			if (!Process())
			{
				Exit();
			}
		}

		if (mode_changed && next_mode == ModeType::Startup)
		{
			if (!Startup())
			{
				Exit();
			}
		}

		if (mode_changed && next_mode == ModeType::Initialize)
		{
			if (!Initialize())
			{
				Exit();
			}
		}

		if (mode_changed && next_mode == ModeType::Freeze)
		{
			if (!Freeze())
			{
				Exit();
			}
		}

		if (mode_changed && next_mode == ModeType::Run)
		{
			if (!Run())
			{
				Exit();
			}
		}

		if (mode_changed && next_mode == ModeType::Shutdown)
		{
			if (!Shutdown())
			{
				Exit();
			}
		}

		if (mode_changed && next_mode == ModeType::Exit)
		{
			Exit();
		}
	}
	LOGFMTT("End MultiNodesKernel ExecuteKernel name %s kernel_mode %d\n", kernel_name.c_str(), kernel_mode);
}
