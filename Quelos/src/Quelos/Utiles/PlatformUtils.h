#pragma once

#include <string>

namespace Quelos
{
	class FileDialogs
	{
	public:
		// These returns empty string if canceled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}
