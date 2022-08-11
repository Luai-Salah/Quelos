#include "QSPCH.h"
#include "GUID.h"

#include <random>

namespace Quelos
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	GUID::GUID()
		: m_GUID(s_UniformDistribution(s_Engine))
	{
	}

	GUID::GUID(uint64_t guid)
		: m_GUID(guid)
	{
	}
}
