
#pragma once

#include "predef.hpp"
#include <string.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/preprocessor.hpp>
#include "utils.hpp"
#include "hash.hpp"
#include "misc.hpp"
#ifdef __linux
#	include <limits.h>
#endif

namespace jebe {
namespace cws {

template<uint8_t plen, uint8_t bits>
class PhraseHash;

template<uint8_t length>
class Phrase
{
	template<uint8_t len> friend bool operator==(const Phrase<len>& lph, const Phrase<len>& rph);
public:
	enum { len = length };
	typedef Phrase<length> P;
	typedef std::equal_to<P> EqualType;

	typedef PhraseHash<length, MapHashBits<length>::bits> MapHashType;
	typedef boost::fast_pool_allocator<P, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 1 << (MapHashBits<length>::bits >> 2)> MapAllocType;

	typedef PhraseHash<length, PadHashBits<length>::bits> PadHashType;
	typedef boost::fast_pool_allocator<P, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 1 << (PadHashBits<length>::bits >> 2)> PadAllocType;

	typedef boost::unordered_map<P, atimes_t, MapHashType, EqualType, MapAllocType> MapType;

	typedef Phrase<1> Suffix;
	typedef std::pair<Suffix, atimes_t> Pad;

	typedef boost::fast_pool_allocator<Pad, boost::default_user_allocator_new_delete,
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

	typedef SumedList<Pad> PadList;
	typedef boost::unordered_map<P, PadList, PadHashType, EqualType, PadAllocType> PadMap;

	typedef CharType StrType[length];

//protected:
	StrType str;

public:
	explicit Phrase(const CharType* const str_)
	{
		staging::memcpy4<length * sizeof(CharType)>(str, str_);
	}

	bool eq(const P& rph) const
	{
		return PhraseMatch<length, length>::match(str, rph.str);
	}

	bool hasPrefix(const Phrase<length - 1>& p) const
	{
		return PhraseMatch<length - 1, length>::match(p.str, str);
	}

	operator String() const
	{
		String s;
		s.assign(str, length);
		return s;
	}

	operator uint16_t() const
	{
		return reinterpret_cast<const uint16_t*>(str)[BOOST_PP_IF(CS_IS_LITTLE_ENDIAN, 0, 1)];
	}

#if CS_DEBUG || CS_LOG_ON
	CharType* c_str() const
	{
		CharType* const cstr = new CharType[length + 1];
		memset(cstr, 0, (length + 1) * sizeof(CharType));
		memcpy(cstr, str, length * sizeof(CharType));
		return cstr;
	}
#endif
};

#define _JEBE_DEF_PHRASE(Z, n, N)		typedef Phrase<n> BOOST_PP_CAT(Ph, n);
BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_ASCII_WORD_MAX_LEN, 2), _JEBE_DEF_PHRASE, BOOST_PP_EMPTY());
#undef _JEBE_DEF_PHRASE

//template<uint8_t len_1, uint8_t len_2> CS_FORCE_INLINE
//bool match(const CharType prefix[len_1], const CharType rstr[len_2]);

template<uint8_t len> CS_FORCE_INLINE
bool operator==(const Phrase<len>& lph, const Phrase<len>& rph)
{
	return PhraseMatch<len, len>::match(lph.str, rph.str);
}

template<uint8_t plen> CS_FORCE_INLINE
uint32_t hfhash(const Phrase<plen>& p);

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

} /* namespace fts */
} /* namespace jebe */
