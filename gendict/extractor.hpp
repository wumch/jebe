
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

public:
	void extract(const PathList& contentfiles, const boost::filesystem::path& outfile);

	void display();

	Extractor(const boost::filesystem::path& gbfile);

protected:
	bool isAscii(const CharType c) const
	{
		return c < 128;
//		return CS_BLIKELY(c > 127) ? false : (L'a' <= c && c <= L'z') || (L'0' <= c && c <= L'9') || (c == L'-');
	}

	void fetchContent(const PathList& contentfiles);

	void dump(const boost::filesystem::path& outfile);

	void scan(CharType* const str, String::size_type len);

	void addSentence(CharType* const str, String::size_type len);

	template<uint8_t plen>
	void scanSentence_(CharType* const str, String::size_type len,
			typename PhraseTrait<plen>::MapType& map);

	bool isGb2312(uint16_t c) const
	{
		BOOST_STATIC_ASSERT(_JEBE_GB_CHAR_MAX >= USHRT_MAX);
		return gb2312[c];
	}
};

} /* namespace cws */
} /* namespace jebe */
