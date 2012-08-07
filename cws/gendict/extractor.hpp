
#pragma once

#define CS_DEBUG	0
#define CS_LOG_ON	0
#define CS_USE_WCS	0

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
#include <boost/pool/pool_alloc.hpp>
#include <boost/thread/mutex.hpp>
#include "utils.hpp"
#include "hash.hpp"
#include "misc.hpp"
#ifdef __linux
#	include <limits.h>
#endif

#define _JEBE_WORD_MAX_LEN				7
#define _JEBE_ASCII_WORD_MAX_LEN		20
#define _JEBE_WORD_MIN_ATIMES			10
#define _JEBE_PROCESS_STEP				(2 << 20)

namespace staging {
template<size_t dwords> CS_FORCE_INLINE void memcpy4(void* const s1, const void* const s2);
}

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
		return hasher(hfhash<plen>(ph));
	}
};

#define _JEBE_DEF_PHRASE(Z, n, N)		typedef Phrase<n> BOOST_PP_CAT(Ph, n);
BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_ASCII_WORD_MAX_LEN, 2), _JEBE_DEF_PHRASE, BOOST_PP_EMPTY());
#undef _JEBE_DEF_PHRASE

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
	typedef boost::unordered_map<String, atimes_t> Words;

protected:
	typedef boost::array<atimes_t, Extractor::gb_char_max> SuffixMap;
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

	boost::array<uint64_t, BOOST_PP_INC(_JEBE_WORD_MAX_LEN)> totalAtimes;

	Words words;

	static const double entropyThresholdLower	= 0.3;
	static const double entropyThresholdUpper	= 1.5;
	static const double joinThresholdLower		= 50.;
	static const double joinThresholdUpper		= 1000.;
	static const uint32_t atimesThreshold		= 200;

	enum WordExamineRes {
		no 					= 1,					// it's not a word.
		yes 				= 1 << 1,				// it's a word.
		typo_prefix 		= 1 << 2, 							// it's a word, and str[:-1] is a typo.
		typo_suffix 		= 1 << 3,							// it's a word, and str[1:] is a typo.
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
				words[it->first] = it->second;
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
				CS_SAY("first " << prefix_len << ", prefix: [" << prefix << "], suffix: [" << suffix << "], atimes: " << it->second);
			}
			else
			{
				padit->second = it->second;
				CS_SAY("repeat " << prefix_len << ", prefix: [" << prefix << "], suffix: " << suffix << "], atimes: " << it->second);
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
				CS_SAY("first " << prefix_len << ", prefix: [" << suffix << "], suffix: [" << prefix << "], atimes: " << it->second);
			}
			else
			{
				padit->second = it->second;
				CS_SAY("repeat " << prefix_len << ", prefix: [" << suffix << "], suffix: " << prefix << "], atimes: " << it->second);
			}
			CS_SAY("plist.sum: " << plist.sum << " (" << &plist << ")");
		}
	}

	void buildSuffixMap()
	{
		typedef Phrase<1> PhraseType;
		typedef PhraseType::Suffix::MapType MapType;
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
