#pragma once

#include "Quelos/Core/Buffer.h"

namespace Quelos
{
	class FileSystem
	{
	public:
		static Buffer ReadBytes(const std::filesystem::path& filepath);

	private:

	};
}
