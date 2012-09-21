
#pragma once

#include "predef.hpp"
#ifdef __linux
#	include <limits.h>
#endif
#include <vector>
#include <bitset>
#include <iostream>
#include <string.h>
#include <boost/filesystem.hpp>
#include <boost/preprocessor.hpp>
#include "hash.hpp"
#include "memory.hpp"
#include "misc.hpp"
#include "utils.hpp"
#include "phrase.hpp"
#include "phrasetrait.hpp"

namespace jebe {
namespace cws {

class Extractor
{
public:
	typedef std::vector<boost::filesystem::path> PathList;

protected:
	std::bitset<_JEBE_GB_CHAR_MAX> gb2312;

	#define _JEBE_DECL_MAP(Z, n, N)		PhraseTrait<n>::MapType BOOST_PP_CAT(map, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DECL_MAP, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_MAP

	LatinPhraseMapType latinp;
	PrependPhraseMapType pwords;

public:
	void extract(const PathList& contentfiles, const boost::filesystem::path& outfile);

	void display();

	Extractor(const boost::filesystem::path& gbfile);

protected:
	bool CS_FORCE_INLINE isAscii(const CharType c) const
	{
		return c < 128;
	}

	bool CS_FORCE_INLINE isAsciiStrict(const CharType c) const
	{
		return CS_BLIKELY(c > 127) ? false : (L'a' <= c && c <= L'z') || (L'0' <= c && c <= L'9') || (c == L'-');
	}

	bool CS_FORCE_INLINE isAllDigit(const CharType* s, String::size_type len) const
	{
		for (uint i = 0; i < len; ++i)
		{
			if (CS_BLIKELY(s[i] <='0' || '9' <= s[i]))
			{
				return false;
			}
		}
		return true;
	}

	template<typename _CharType> CS_FORCE_INLINE
	void replacetolower(_CharType* c) const
	{
		if (CS_BUNLIKELY(isUpperAlpha(*c)))
		{
			 *c |= 32;
		}
	}

	template<typename _CharType> CS_FORCE_INLINE
	bool isUpperAlpha(_CharType c) const
	{
		return 'A' <= c && c <= 'Z';
	}

	void fetchContent(const PathList& contentfiles);

	void dump(const boost::filesystem::path& outfile);

	void scan(CharType* const str, String::size_type len);

	void addSentence(CharType* const str, String::size_type len);

	bool scanLatin(CharType* const str, String::size_type len);

	void scanPrepends(CharType* const str, String::size_type len);

	template<uint8_t plen>
	void scanSentence_(CharType* const str, String::size_type len,
			typename PhraseTrait<plen>::MapType& map);

	bool isGb2312(uint16_t c) const
	{
		BOOST_STATIC_ASSERT(_JEBE_GB_CHAR_MAX > USHRT_MAX);
		return gb2312[c];
	}
};

} /* namespace cws */
} /* namespace jebe */
