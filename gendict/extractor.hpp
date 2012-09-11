
#pragma once

#include "predef.hpp"
#ifdef __linux
#	include <limits.h>
#endif
#include <bitset>
#include <list>
#include <iostream>
#include <string.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/preprocessor.hpp>
#include <boost/array.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/thread/mutex.hpp>
#include "utils.hpp"
#include "hash.hpp"
#include "misc.hpp"

namespace staging {
template<size_t dwords> CS_FORCE_INLINE void memcpy4(void* const s1, const void* const s2);
}

namespace jebe {
namespace cws {

class Extractor
{
public:
	static const int32_t gb_char_max = 65536;

	typedef std::vector<boost::filesystem::path> PathList;

protected:
	std::bitset<gb_char_max> gb2312;

	#define _JEBE_DECL_MAP(Z, n, N)		BOOST_PP_CAT(Ph, n)::MapType BOOST_PP_CAT(map, n);
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
		return CS_BLIKELY(c > 127) ? false : (L'a' <= c && c <= L'z') || (L'0' <= c && c <= L'9') || (c == L'-');
	}

	void fetchContent(const PathList& contentfiles);

	void dump(const boost::filesystem::path& outfile);

	void scan(CharType* const str, String::size_type len);

	void addSentence(CharType* const str, String::size_type len);

	template<uint8_t plen>
	void scanSentence_(CharType* const str, String::size_type len,
			typename Phrase<plen>::MapType& map);

	bool isGb2312(CharType c) const
	{
		return c >= 0 && c < gb_char_max && gb2312[c];
	}
};

template<uint8_t prefix_len>
bool padEq(const typename Phrase<prefix_len>::Pad& p1, const typename Phrase<prefix_len>::Pad& p2)
{
	return PhraseMatch<prefix_len, prefix_len>::match(p1.first.str, p2.first.str);	// Phrase
}

} /* namespace cws */
} /* namespace jebe */
