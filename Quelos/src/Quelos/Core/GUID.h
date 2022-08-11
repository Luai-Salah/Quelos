#pragma once

namespace Quelos
{
	class GUID
	{
	public:
		GUID();
		GUID(uint64_t guid);
		GUID(const GUID&) = default;

		operator uint64_t() const { return m_GUID; }
	private:
		uint64_t m_GUID;
	};
}

namespace std
{
	template<>
	struct hash<Quelos::GUID>
	{
		std::size_t operator()(const Quelos::GUID& guid) const
		{
			return (uint64_t)guid;
		}
	};
}
