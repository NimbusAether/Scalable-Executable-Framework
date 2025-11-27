#include "Kernel.h"
#include "log.h"
using SEF::Kernel::Kernel;
using namespace SEF::Kernel;

Kernel::Kernel()
{
	kernel_name = "";
	kernel_mode = ModeType::Null;
	kernel_type = KernelType::None;
	mode_changed = true;
	next_mode = ModeType::Idle;
}

Kernel::~Kernel() 
{
	
}

void Kernel::SetKernel(KernelType kernelType, string kernelName) 
{
	if (kernelName != "")
	{
		kernel_type = kernelType;
		kernel_name = kernelName;
	}
}

void Kernel::ExecuteKernel() 
{
	LOGFMTT("Begin ExecuteKernel name %s kernel_mode %d\n", kernel_name.c_str(), kernel_mode);
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
		
		if (mode_changed && next_mode == ModeType::Exit)
		{
			Exit();
		}
	}
	LOGFMTT(" End ExecuteKernel name %s kernel_mode %d\n", kernel_name.c_str(), kernel_mode);
}

string Kernel::GetKernelName()
{
	return kernel_name;
}

ModeType Kernel::GetKernelMode() const 
{
	return kernel_mode;
}

KernelType Kernel::GetKernelType() const
{
	return kernel_type;
}

bool Kernel::Idle() 
{
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Idle Mode." << endl;
		kernel_mode = ModeType::Idle;
		mode_changed = true;
		next_mode = ModeType::Process;
		return true;
	}
	catch (exception) {
		return false;
	}
	
}

bool Kernel::Process() {
	try {
		mode_changed = false;
		cout << kernel_name << "|Enter Process Mode." << endl;
		kernel_mode = ModeType::Process;
		mode_changed = true;
		next_mode = ModeType::Exit;
		return true;
	}
	catch (exception) {
		return false;
	}
	
}

void Kernel::Exit() 
{
	mode_changed = false;
	cout << kernel_name << "|Enter Exit Mode." << endl;
	kernel_mode = ModeType::Exit;
	next_mode = ModeType::Null;
	exit(0);
}
