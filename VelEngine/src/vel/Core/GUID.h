#pragma once
#include <xhash>

namespace vel
{
	//globally unique identifier
	class GUID
	{
	public:
		GUID();
		GUID(uint64_t guid);
		GUID(const GUID& other);

		operator uint64_t () { return m_GUID; }
		operator const uint64_t() const { return m_GUID; }

	private:
		uint64_t m_GUID;
	};
}


namespace std {

	template <>
	struct hash<vel::GUID>
	{
		std::size_t operator()(const vel::GUID& guid) const
		{
			// guid is already a randomly generated number, and is suitable as a hash key as-is.
			// this may change in future, in which case return hash<uint64_t>{}(guid); might be more appropriate
			return guid;
		}
	};
}