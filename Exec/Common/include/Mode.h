#pragma once
#include <vector>
#include <string>

namespace SEF {
	namespace Kernel {
		enum class ModeType
		{
			Null = -1,
			Idle = 0,
			Process = 1,
			Startup = 2,
			Initialize = 3,
			Freeze = 4,
			Run = 5,
			Shutdown = 6,
			Exit = 7
		};
		inline const char* modeType2String(ModeType type) {
			switch (type) {
			case ModeType::Null:
				return "Null";
			case ModeType::Exit:
				return "Exit";
			case ModeType::Freeze:
				return "Freeze";
			case ModeType::Idle:
				return "Idle";
			case ModeType::Initialize:
				return "Initialize";
			case ModeType::Process:
				return "Process";
			case ModeType::Run:
				return "Run";
			case ModeType::Shutdown:
				return "Shutdown";
			case ModeType::Startup:
				return "Startup";
			default:
				return "unknown";
			}
		}
	}
}
