#pragma once

namespace SEF
{
	namespace Kernel
	{
		enum class KernelType
		{
			None = -1,
			OneShot = 0,
			Batch = 1,
			Scheduled = 2,
			Daemon = 3,
			Interactive = 4,
			Centralization = 5,
			Distributed = 6,
			Network = 7
		};
	}
}
