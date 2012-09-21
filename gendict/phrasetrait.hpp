
#pragma once

#include "predef.hpp"
#ifdef __linux
#	include <limits.h>
#endif
#include <string.h>
#include <boost/unordered_map.hpp>
#include <boost/preprocessor.hpp>
#include <boost/pool/pool_alloc.hpp>
#include "hash.hpp"
#include "misc.hpp"
#include "memory.hpp"
#include "utils.hpp"
#include "phrase.hpp"

namespace jebe {
namespace cws {

template<uint8_t n> CS_FORCE_INLINE static
uint32_t hfhash(const Phrase<n>& p)
{
	unsigned int mask = 0;
	char* res = reinterpret_cast<char*>(&mask);
	const char* data = reinterpret_cast<const char*>(p.str);
	for (uint i = 0; i < (n); ++i)
	{
		res[i & 3] ^= p.str[i];
	}
	return mask;
}

template<> CS_FORCE_INLINE
uint32_t hfhash<1>(const Phrase<1>& p)
{
	return static_cast<uint16_t>(p.str[0]);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<2>(const Phrase<2>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<3>(const Phrase<3>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<4>(const Phrase<4>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<5>(const Phrase<5>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<6>(const Phrase<6>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<7>(const Phrase<7>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16)  ^ (static_cast<uint16_t>(p.str[6]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<8>(const Phrase<8>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16)  ^ (static_cast<uint16_t>(p.str[6]) << 8)  ^ (static_cast<uint16_t>(p.str[7]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<9>(const Phrase<9>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16)  ^ static_cast<uint16_t>(p.str[6]) ^ (static_cast<uint16_t>(p.str[7]) << 8)  ^ (static_cast<uint16_t>(p.str[8]) << 16);
}

template<uint8_t plen, uint8_t bits>
class PhraseHash
{
	BOOST_STATIC_ASSERT(bits <= (sizeof(std::size_t) * CHAR_BIT));
public:
	uint32_t operator()(const Phrase<plen>& ph) const
	{
		return hfhash<plen>(ph);
	}
};

template<uint8_t length>
class PhraseTrait
{
public:
	typedef Phrase<length> PhraseType;
	typedef std::equal_to<PhraseType> EqualType;

	typedef Phrase<1> SuffixType;
	typedef std::pair<SuffixType, atimes_t> PadType;

	typedef Phrase<length - 1> PrefixType;

	typedef PhraseHash<length, MapHashBits<length>::bits> MapHashType;
	typedef boost::fast_pool_allocator<PhraseType, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 1 << (MapHashBits<length>::bits >> 2)> MapAllocType;

	typedef boost::unordered_map<PhraseType, atimes_t, MapHashType, EqualType, MapAllocType> MapType;

	typedef PhraseHash<length, PadHashBits<length>::bits> PadHashType;
	typedef boost::fast_pool_allocator<PhraseType, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 1 << (PadHashBits<length>::bits >> 2)> PadAllocType;


	typedef boost::fast_pool_allocator<PadType, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 6763> ListElemAllocType;

	template<typename T>
	class SumedList
	{
	public:
		typedef std::list<T, ListElemAllocType> List;
		typedef typename List::iterator iterator;
		typedef typename List::const_iterator const_iterator;

		std::size_t sum;
		List list;

		SumedList(): sum(0) {}

		void append(const T& elem)
		{
			list.push_back(elem);
			sum += elem.second;
		}

		const List* operator->() const
		{
			return &list;
		}

		List* operator->()
		{
			return &list;
		}
	};

	typedef SumedList<PadType> PadList;
	typedef boost::unordered_map<PhraseType, PadList, PadHashType, EqualType, PadAllocType> PadMap;
};

typedef PhraseTrait<1> SuffixTrait;

template<uint8_t plen>
class PadEqual
{
private:
	typedef PhraseTrait<plen> Trait;
	const typename Trait::PhraseType& phrase;

public:
	explicit PadEqual(const typename Trait::PhraseType& p): phrase(p) {}

	bool operator()(const typename Trait::PadType& p) const
	{
		return p.first == phrase;
	}
};

/**
 * latin-phrase, non-fixed-length
 */
typedef std::string LatinPhrase;
typedef std::equal_to<LatinPhrase> LatinPhraseEqual;
typedef staging::Hflp<_JEBE_LATIN_PHRASE_NUM_EXPCET_BITS, LatinPhrase> LatinPhraseHash;
typedef boost::fast_pool_allocator<LatinPhrase, boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1 << _JEBE_LATIN_PHRASE_NUM_EXPCET_BITS> LatinPhraseAllocType;
typedef boost::unordered_map<LatinPhrase, atimes_t, LatinPhraseHash, LatinPhraseEqual, LatinPhraseAllocType> LatinPhraseMapType;

/**
 * prepend-words, non-fixed-length
 */
typedef String PrependPhrase;
typedef staging::Hflp<_JEBE_PRE_WORDS_NUM_EXPCET_BITS, PrependPhrase> PrependPhraseHash;
typedef std::equal_to<PrependPhrase> PrependPhraseEqual;
typedef boost::fast_pool_allocator<PrependPhrase, boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1 << _JEBE_PRE_WORDS_NUM_EXPCET_BITS> PrependPhraseAllocType;
typedef boost::unordered_map<PrependPhrase, atimes_t, PrependPhraseHash, PrependPhraseEqual, PrependPhraseAllocType> PrependPhraseMapType;

}
}
