
#pragma once

#include "staging.hpp"
#include <bitset>
#include <list>
#include <iostream>
#include <string.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/preprocessor.hpp>
#include "hash.hpp"
#include "array.hpp"
#include "misc.hpp"
#ifndef WIN32
#	include <limits.h>
#endif

#define _JEBE_WORD_MAX_LEN		7
#define _JEBE_WORD_MIN_ATIMES	5
#define _JEBE_PROCESS_STEP		(2 << 20)

namespace jebe {
namespace cws {

typedef uint32_t atimes_t;
typedef wchar_t	CharType;
BOOST_STATIC_ASSERT(sizeof(CharType) == 4);
typedef std::wstring String;

template<uint8_t plen> class MapHashBits { public: enum { bits = 13 }; };
template<> class MapHashBits<1> { public: enum { bits = 12 }; };
template<> class MapHashBits<2> { public: enum { bits = 24 }; };
template<> class MapHashBits<3> { public: enum { bits = 24 }; };
template<> class MapHashBits<4> { public: enum { bits = 22 }; };
template<> class MapHashBits<5> { public: enum { bits = 20 }; };
template<> class MapHashBits<6> { public: enum { bits = 18 }; };
template<> class MapHashBits<7> { public: enum { bits = 15 }; };

template<uint8_t plen> class PadHashBits { public: enum { bits = 12 }; };
template<> class PadHashBits<1> { public: enum { bits = MapHashBits<1>::bits }; };
template<> class PadHashBits<2> { public: enum { bits = 22 }; };
template<> class PadHashBits<3> { public: enum { bits = 20 }; };
template<> class PadHashBits<4> { public: enum { bits = 18 }; };
template<> class PadHashBits<5> { public: enum { bits = 16 }; };
template<> class PadHashBits<6> { public: enum { bits = 14 }; };

template<uint8_t plen, uint8_t bits>
class PhraseHash;

template<uint8_t length>
class Phrase
{
	template<uint8_t len> friend bool operator==(const Phrase<len>& lph, const Phrase<len>& rph);
public:
	typedef Phrase<length> P;

	typedef boost::unordered_map<P, atimes_t, PhraseHash<length, MapHashBits<length>::bits> > MapType;

	typedef Phrase<1> Suffix;
	typedef std::pair<Suffix, atimes_t> Pad;

	template<typename T>
	class SumedList
	{
	public:
		typedef std::list<T> List;
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

		List* operator->()
		{
			return &list;
		}
	};

	typedef SumedList<Pad> PadList;
	typedef boost::unordered_map<P, PadList, PhraseHash<length, PadHashBits<length>::bits> > PadMap;

	static const uint8_t len = length;
	typedef CharType StrType[length];

//protected:
	StrType str;

public:
	explicit Phrase(const CharType* const str_)
	{
		memcpy(str, str_, length * sizeof(CharType));
	}

	bool eq(const P& rph) const
	{
		return match(str, rph.str);
	}

	bool hasPrefix(const Phrase<length - 1>& p) const
	{
		return match(p.str, str);
	}

	operator String() const
	{
		String s;
		s.assign(str, length);
		return s;
	}

	operator uint16_t() const
	{
		return reinterpret_cast<const uint64_t*>(str)[BOOST_PP_IF(CS_IS_LITTLE_ENDIAN, 0, 1)];
	}

#if CS_DEBUG
	CharType* c_str() const
	{
		CharType* cstr = new CharType[length + 1];
		memset(cstr, 0, (length + 1) * sizeof(CharType));
		memcpy(cstr, str, length * sizeof(CharType));
		return cstr;
	}
#endif
};

template<uint8_t len_1, uint8_t len_2> CS_FORCE_INLINE
bool match(const CharType prefix[len_1], const CharType rstr[len_2]);

template<uint8_t len> CS_FORCE_INLINE
bool operator==(const Phrase<len>& lph, const Phrase<len>& rph)
{
	return match<len, len>(lph.str, rph.str);
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
		return hasher(hfhash(ph));
	}
};

#define _JEBE_DEF_PHRASE(Z, n, N)		typedef Phrase<n> BOOST_PP_CAT(Ph, n);
BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DEF_PHRASE, BOOST_PP_EMPTY());
#undef _JEBE_DEF_PHRASE

class Extractor
{
public:
	static const int32_t gb_char_max = 65536;

protected:
	std::bitset<gb_char_max> gb2312;

	#define _JEBE_DECL_MAP(Z, n, N)		BOOST_PP_CAT(Ph, n)::MapType BOOST_PP_CAT(map, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DECL_MAP, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_MAP

public:
	void extract(const boost::filesystem::path& contentfile,
			const boost::filesystem::path& outfile, uint32_t max_chars);

	void display();

	Extractor(const boost::filesystem::path& gbfile);

protected:
	void fetchContent(const boost::filesystem::path& contentfile,
			const boost::filesystem::path& outfile, uint32_t max_chars);

	void extract(const boost::filesystem::path& outfile);

	void scan(const CharType* const str, String::size_type len);

	void addSentence(const CharType* const str, String::size_type len);

	template<uint8_t plen>
	void scanSentence_(const CharType* const str, String::size_type len,
			typename Phrase<plen>::MapType& map);

	bool isGb2312(CharType c) const
	{
		return c >= 0 && c < gb_char_max && gb2312[c];
	}
};

template<uint8_t prefix_len>
bool padEq(const typename Phrase<prefix_len>::Pad& p1, const typename Phrase<prefix_len>::Pad& p2)
{
	return match<prefix_len, prefix_len>(p1.first.str, p2.first.str);	// Phrase
}

template<uint8_t plen>
class PadEqual
{
private:
	typedef Phrase<plen> P;
	const P& phrase;

public:
	explicit PadEqual(const P& p): phrase(p) {}

	bool operator()(const typename P::Pad& p) const
	{
		return p.first == phrase;
	}
};

class Analyzer
{
public:
	typedef boost::unordered_set<String> Words;

protected:
	typedef staging::Array<atimes_t, Extractor::gb_char_max> SuffixMap;
	SuffixMap smap;

	#define _JEBE_DECL_MAP(Z, n, N)		BOOST_PP_CAT(Ph, n)::MapType& BOOST_PP_CAT(map, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DECL_MAP, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_MAP

	#define _JEBE_DECL_PAD(Z, n, N)		BOOST_PP_CAT(Ph, n)::PadMap BOOST_PP_CAT(pad, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_WORD_MAX_LEN), _JEBE_DECL_PAD, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_PAD

	std::size_t totalAtimes;

	Words words;

	static const double entropyThreshold		= 0.2;
	static const double joinThresholdLower		= 10.;
	static const double joinThresholdUpper		= 300.;

	enum WordExamineRes { no = 0, yes, should_cover, };

public:
	#define _JEBE_ANALYZER_ARG(Z, n, N)			BOOST_PP_CAT(Ph, n)::MapType& BOOST_PP_CAT(BOOST_PP_CAT(map, n), _)BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
	#define _JEBE_ANALYZER_INIT(Z, n, N)		BOOST_PP_CAT(map, n)(BOOST_PP_CAT(BOOST_PP_CAT(map, n), _)),
	Analyzer(BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_ANALYZER_ARG, BOOST_PP_EMPTY()))
		: BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_ANALYZER_INIT, BOOST_PP_EMPTY())
		  totalAtimes(0)
	{
		buildSuffixMap();
	}
	#undef _JEBE_ANALYZER_ARG
	#undef _JEBE_ANALYZER_INIT

	Words& getWords()
	{
		return words;
	}

	void extractWords()
	{
		#define _JEBE_CALL_EXTRACT_WORDS(Z, n, N)		extractWords_<n>(BOOST_PP_CAT(map, n), BOOST_PP_CAT(map, BOOST_PP_DEC(n)), BOOST_PP_CAT(pad, BOOST_PP_DEC(n)));
		BOOST_PP_REPEAT_FROM_TO(2, BOOST_PP_INC(_JEBE_WORD_MAX_LEN), _JEBE_CALL_EXTRACT_WORDS, BOOST_PP_EMPTY())
		#undef _JEBE_CALL_EXTRACT_WORDS
		CS_SAY("character-total-atimes: " << totalAtimes);
	}

	template<uint8_t plen>
	void extractWords_(typename Phrase<plen>::MapType& map,
		typename Phrase<plen - 1>::MapType& prefixmap,
		typename Phrase<plen - 1>::PadMap& padmap
		)
	{
		BOOST_STATIC_ASSERT(plen > 1);

		WordExamineRes res = no;
		for (typename Phrase<plen>::MapType::const_iterator it = map.begin(); it != map.end(); ++it)
		{
			res = isWord<plen>(it->first, map, prefixmap, padmap);
			if (res != no)
			{
				words.insert(it->first);
				if (res == should_cover)
				{
					words.erase(Phrase<plen - 1>(it->first.str));
				}
			}	// else: could remove some longer phrases for performance.
		}
	}

	template<uint8_t plen>
	Analyzer::WordExamineRes isWord(const Phrase<plen>& phrase,
			typename Phrase<plen>::MapType& map,
			typename Phrase<plen - 1>::MapType& prefixmap,
			typename Phrase<plen - 1>::PadMap& padmap
			) const;

	void analysis();

	void caltureTotalAtimes()
	{
		for (Ph1::MapType::const_iterator it = map1.begin(); it != map1.end(); ++it)
		{
			totalAtimes += it->second;
		}
	}

	void buildPadMap()
	{
		#define _JEBE_CALL_BUILD_MAP(Z, n, N)		buildPadMap_<n>(BOOST_PP_CAT(pad, n), BOOST_PP_CAT(map, BOOST_PP_INC(n)));
		BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_WORD_MAX_LEN), _JEBE_CALL_BUILD_MAP, BOOST_PP_EMPTY())
		#undef _JEBE_CALL_BUILD_MAP
	}

	template<uint8_t prefix_len>
	void buildPadMap_(typename Phrase<prefix_len>::PadMap& padmap, const typename Phrase<prefix_len + 1>::MapType& map)
	{
		typedef Phrase<prefix_len> PrefixType;
		typedef typename PrefixType::Suffix SuffixType;
		typedef typename Phrase<prefix_len + 1>::MapType MapType;

		typedef typename PrefixType::PadList PadListType;
		typedef typename PrefixType::PadMap PadMapType;

		for (typename MapType::const_iterator it = map.begin(); it != map.end(); ++it)
		{
			PrefixType prefix(it->first.str);
			SuffixType suffix(it->first.str + prefix_len);

			if (padmap.find(prefix) == padmap.end())
			{
				padmap[prefix] = PadListType();
			}

			PadListType& plist = padmap[prefix];
			typename PadListType::iterator padit =std::find_if(plist->begin(), plist->end(), PadEqual<1>(suffix));
			if (padit == plist->end())
			{
				plist.append(typename PrefixType::Pad(suffix, it->second));
				CS_SAY("first " << prefix_len << ", prefix: [" << prefix.c_str() << "], suffix: [" << suffix.c_str() << "], atimes: " << it->second);
			}
			else
			{
				padit->second = it->second;
				CS_SAY("repeat " << prefix_len << ", prefix: [" << prefix.c_str() << "], suffix: " << suffix.c_str() << "], atimes: " << it->second);
			}
			CS_SAY("plist.sum: " << plist.sum << " (" << &plist << ")");
		}
	}

	void buildSuffixMap()
	{
		typedef Phrase<1> PhraseType;
		typedef PhraseType::Suffix::MapType MapType;
		smap.fill(0);
		for (MapType::const_iterator it = map1.begin(); it != map1.end(); ++it)
		{
			smap[it->first] = it->second;
		}
	}

	void clean(std::size_t min_atimes);

	template<uint8_t plen>
	void clean_(typename Phrase<plen>::MapType& map, std::size_t min_atimes);

	bool wordJudge() const;

	template<uint8_t plen>
	void getSuffixCount(const Phrase<plen>& ph,
			const typename Phrase<plen>::MapType& map,
			const typename Phrase<plen + 1>::MapType& suffixMap
			) const
	{
	}
};

} /* namespace cws */
} /* namespace jebe */
