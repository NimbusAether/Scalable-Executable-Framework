#pragma once
#include "KernelType.h"
#include "Mode.h"
#include <string>
#include <iostream>
#include "base.h"
using namespace std;

namespace SEF
{
	namespace Kernel
	{
		class UDM_EXPORT_API Kernel
		{
		public:
			Kernel();
			virtual ~Kernel();
			void SetKernel(KernelType kernelType, string kernelName);
			virtual void ExecuteKernel();
			string GetKernelName();
			ModeType GetKernelMode() const;
			KernelType GetKernelType() const;
		protected:
			bool Idle();
			virtual bool Process();
			void Exit();
			string kernel_name;
			ModeType kernel_mode;
			KernelType kernel_type;
			bool mode_changed;
			ModeType next_mode;
		};
	}

}