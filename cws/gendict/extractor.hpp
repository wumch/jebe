
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
#include <boost/array.hpp>
#include "hash.hpp"
#include "array.hpp"
#include "misc.hpp"
#ifdef __linux
#	include <limits.h>
#endif

#define _JEBE_WORD_MAX_LEN		5
#define _JEBE_ASCII_WORD_MAX_LEN		20
#define _JEBE_WORD_MIN_ATIMES	5
#define _JEBE_PROCESS_STEP		(2 << 20)

namespace staging {

template<size_t dwords> CS_FORCE_INLINE
void memcpy4(void* const s1, const void* const s2);

}

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
	enum { len = length };
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
	typedef boost::unordered_map<P, PadList, PhraseHash<length, PadHashBits<length>::bits> > PadMap;

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
BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_ASCII_WORD_MAX_LEN, 2), _JEBE_DEF_PHRASE, BOOST_PP_EMPTY());
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
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_ASCII_WORD_MAX_LEN), _JEBE_DECL_PAD, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_PAD

	#define _JEBE_DECL_PAD(Z, n, N)		BOOST_PP_CAT(Ph, n)::PadMap BOOST_PP_CAT(prx, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_ASCII_WORD_MAX_LEN), _JEBE_DECL_PAD, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_PAD

	staging::Array<uint64_t, BOOST_PP_INC(_JEBE_WORD_MAX_LEN)> totalAtimes;

	Words words;

	static const double entropyThresholdLower	= 0.2;
	static const double entropyThresholdUpper	= 1.5;
	static const double joinThresholdLower		= 10.;
	static const double joinThresholdUpper		= 600.;
	static const double firstCharMaxMiss		= 80;	// atimes(str[0]) < 50*atimes(str[1:]

	enum WordExamineRes {
		no 					= 1,					// it's not a word.
		yes 				= 1 << 1,				// it's a word.
		typo_prefix 		= 1 << 2, 							// it's a word, and str[:-1] is not a typo.
		typo_suffix 		= 1 << 3,							// it's a word, and str[1:] is not a typo.
		typo_prefix_suffix 	= yes | typo_prefix | typo_suffix,	// it's a word, and both str[:-1] and str[1:] are typo.
	};

public:
	#define _JEBE_ANALYZER_ARG(Z, n, N)			BOOST_PP_CAT(Ph, n)::MapType& BOOST_PP_CAT(BOOST_PP_CAT(map, n), _)BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
	#define _JEBE_ANALYZER_INIT_MAP(Z, n, N)		BOOST_PP_CAT(map, n)(BOOST_PP_CAT(BOOST_PP_CAT(map, n), _)),
	#define _JEBE_ANALYZER_INIT_PAD(Z, n, N)		BOOST_PP_CAT(pad, n)(1 << PadHashBits<n>::bits),
	#define _JEBE_ANALYZER_INIT_PRX(Z, n, N)		BOOST_PP_CAT(prx, n)(1 << PadHashBits<n>::bits)BOOST_PP_COMMA_IF(BOOST_PP_LESS(n, _JEBE_WORD_MAX_LEN))
	Analyzer(BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_ANALYZER_ARG, BOOST_PP_EMPTY()))
		: BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_ANALYZER_INIT_MAP, BOOST_PP_EMPTY())
		  BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_WORD_MAX_LEN), _JEBE_ANALYZER_INIT_PAD, BOOST_PP_EMPTY())
		  BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_WORD_MAX_LEN), _JEBE_ANALYZER_INIT_PRX, BOOST_PP_EMPTY())
	{
		buildSuffixMap();
	}
	#undef _JEBE_ANALYZER_ARG
	#undef _JEBE_ANALYZER_INIT

	void analysis();

	const Words& getWords() const
	{
		return words;
	}

protected:
	void extractWords()
	{
		#define _JEBE_CALL_EXTRACT_WORDS(Z, n, N)		extractWords_<n>(BOOST_PP_CAT(map, n), BOOST_PP_CAT(map, BOOST_PP_DEC(n)), BOOST_PP_CAT(pad, BOOST_PP_DEC(n)), BOOST_PP_CAT(prx, BOOST_PP_DEC(n)));
		BOOST_PP_REPEAT_FROM_TO(2, BOOST_PP_INC(_JEBE_WORD_MAX_LEN), _JEBE_CALL_EXTRACT_WORDS, BOOST_PP_EMPTY())
		#undef _JEBE_CALL_EXTRACT_WORDS
//		CS_SAY("character-total-atimes: " << totalAtimes);
	}

	template<uint8_t plen> CS_FORCE_INLINE
	bool isTailTypo(const Phrase<plen>& phrase, atimes_t atimes,
		const typename Phrase<plen - 1>::MapType& prefixmap) const;

	template<uint8_t plen>
	void extractWords_(typename Phrase<plen>::MapType& map,
			const typename Phrase<plen - 1>::MapType& prefixmap,
			const typename Phrase<plen - 1>::PadMap& padmap,
			const typename Phrase<plen - 1>::PadMap& prxmap
		)
	{
		BOOST_STATIC_ASSERT(plen > 1);

		typedef Phrase<plen - 1> ShorterPhraseType;
		WordExamineRes res = no;
		for (typename Phrase<plen>::MapType::const_iterator it = map.begin(); it != map.end(); ++it)
		{
			res = judgePad<plen>(it->first, map, prefixmap, padmap);
			if (CS_BUNLIKELY(res & yes))
			{
				words.insert(it->first);
			}
			if (plen > 2)
			{
				if (CS_BUNLIKELY(res & typo_prefix))
				{
					words.erase(ShorterPhraseType(it->first.str));
					CS_SAY("prefix is not a word: [" << it->first.c_str() << "]");
				}

				res = judgePrx<plen>(it->first, map, prefixmap, prxmap);
				if (CS_BUNLIKELY(res & no))
				{
					words.erase(it->first);
				}
//				if (CS_BUNLIKELY(res & yes))
//				{
//					words.insert(it->first);
//				}
				if (CS_BUNLIKELY(res & typo_suffix))
				{
					words.erase(ShorterPhraseType(it->first.str + 1));
					CS_SAY("prefix is not a word: [" << it->first.c_str() << "]");
				}
			}
		}
	}

	template<uint8_t plen>
	Analyzer::WordExamineRes judgePad(const Phrase<plen>& phrase,
			const typename Phrase<plen>::MapType& map,
			const typename Phrase<plen - 1>::MapType& prefixmap,
			const typename Phrase<plen - 1>::PadMap& padmap
		) const;

	template<uint8_t plen>
	Analyzer::WordExamineRes judgePrx(const Phrase<plen>& phrase,
			const typename Phrase<plen>::MapType& map,
			const typename Phrase<plen - 1>::MapType& prefixmap,
			const typename Phrase<plen - 1>::PadMap& padmap
		) const;

	void caltureTotalAtimes()
	{
		totalAtimes.fill(0);
		#define _JEBE_CALL_TOTALATIMES(Z, n, N)		caltureTotalAtimes_<n>(BOOST_PP_CAT(map, n));
		BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_CALL_TOTALATIMES, BOOST_PP_EMPTY())
		#undef _JEBE_CALL_TOTALATIMES
	}

	template<uint8_t plen>
	void caltureTotalAtimes_(const typename Phrase<plen>::MapType& map)
	{
		typedef typename Phrase<plen>::MapType::const_iterator IterType;
		for (IterType it = map.begin(); it != map.end(); ++it)
		{
			totalAtimes[plen - 1] += it->second;
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
			const PrefixType prefix(it->first.str);
			const SuffixType suffix(it->first.str + prefix_len);

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

	void buildPrxMap()
	{
		#define _JEBE_CALL_BUILD_MAP(Z, n, N)		buildPrxMap_<n>(BOOST_PP_CAT(prx, n), BOOST_PP_CAT(map, BOOST_PP_INC(n)));
		BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_WORD_MAX_LEN), _JEBE_CALL_BUILD_MAP, BOOST_PP_EMPTY())
		#undef _JEBE_CALL_BUILD_MAP
	}

	template<uint8_t prefix_len>
	void buildPrxMap_(typename Phrase<prefix_len>::PadMap& prxmap, const typename Phrase<prefix_len + 1>::MapType& map)
	{
		typedef Phrase<prefix_len> SuffixType;
		typedef typename SuffixType::Suffix PrefixType;
		typedef typename Phrase<prefix_len + 1>::MapType MapType;

		typedef typename SuffixType::PadList PadListType;
		typedef typename SuffixType::PadMap PadMapType;

		for (typename MapType::const_iterator it = map.begin(); it != map.end(); ++it)
		{
			const PrefixType prefix(it->first.str);
			const SuffixType suffix(it->first.str + 1);

			if (prxmap.find(suffix) == prxmap.end())
			{
				prxmap[suffix] = PadListType();
			}

			PadListType& plist = prxmap[suffix];
			typename PadListType::iterator padit =std::find_if(plist->begin(), plist->end(), PadEqual<1>(prefix));
			if (padit == plist->end())
			{
				plist.append(typename SuffixType::Pad(prefix, it->second));
				CS_SAY("first " << prefix_len << ", prefix: [" << suffix.c_str() << "], suffix: [" << prefix.c_str() << "], atimes: " << it->second);
			}
			else
			{
				padit->second = it->second;
				CS_SAY("repeat " << prefix_len << ", prefix: [" << suffix.c_str() << "], suffix: " << prefix.c_str() << "], atimes: " << it->second);
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
};

} /* namespace cws */
} /* namespace jebe */
