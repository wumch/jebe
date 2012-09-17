
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

namespace jebe {
namespace cws {

template<uint8_t length>
class Phrase
{
	template<uint8_t len> friend bool operator==(const Phrase<len>& lph, const Phrase<len>& rph);
public:
	enum { len = length };
	typedef Phrase<length> P;

	typedef CharType StrType[length];
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
		return String(str, length);
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

} /* namespace fts */
} /* namespace jebe */
