
#pragma once

#include "predef.hpp"
#ifdef __linux
#	include <limits.h>
#endif
#include "phrase.hpp"

namespace jebe {
namespace cws {

template<uint8_t plen, uint8_t bits>
class PhraseHash
{
	BOOST_STATIC_ASSERT(bits <= (sizeof(std::size_t) * CHAR_BIT));
public:
	typedef Phrase<plen> Ph;
	typedef staging::BitsHash<bits> BHasher;

	static BHasher hasher;

	uint32_t operator()(const Ph& ph) const
	{
		return hfhash<plen>(ph);
	}
};

}
}
