
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
#include "phrase.hpp"
#include "phrasetrait.hpp"
//#include "extractor.impl.hpp"

namespace jebe {
namespace cws {

class Analyzer
{
public:
	typedef boost::unordered_map<String, atimes_t> Words;

protected:
	typedef boost::array<atimes_t, _JEBE_GB_CHAR_MAX> SuffixMap;
	SuffixMap smap;

	#define _JEBE_DECL_MAP(Z, n, N)		PhraseTrait<n>::MapType& BOOST_PP_CAT(map, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DECL_MAP, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_MAP

	#define _JEBE_DECL_PAD(Z, n, N)		PhraseTrait<n>::PadMap BOOST_PP_CAT(pad, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_ASCII_WORD_MAX_LEN), _JEBE_DECL_PAD, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_PAD

	#define _JEBE_DECL_PAD(Z, n, N)		PhraseTrait<n>::PadMap BOOST_PP_CAT(prx, n);
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(_JEBE_ASCII_WORD_MAX_LEN), _JEBE_DECL_PAD, BOOST_PP_EMPTY())
	#undef _JEBE_DECL_PAD

	boost::array<uint64_t, BOOST_PP_INC(_JEBE_WORD_MAX_LEN)> totalAtimes;

	Words words;

	static const double entropyThresholdLower	= 0.3;
	static const double entropyThresholdUpper	= 1.5;
	static const double joinThresholdLower		= 50.;
	static const double joinThresholdUpper		= 100.;
	static const uint32_t atimesThreshold		= 200;

	enum WordExamineRes {
		no 					= 1,					// it's not a word.
		yes 				= 1 << 1,				// it's a word.
		typo_prefix 		= 1 << 2, 							// it's a word, and str[:-1] is a typo.
		typo_suffix 		= 1 << 3,							// it's a word, and str[1:] is a typo.
		typo_prefix_suffix 	= yes | typo_prefix | typo_suffix,	// it's a word, and both str[:-1] and str[1:] are typo.
	};

public:
	#define _JEBE_ANALYZER_ARG(Z, n, N)				PhraseTrait<n>::MapType& BOOST_PP_CAT(BOOST_PP_CAT(map, n), _)BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
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
	}

	template<uint8_t plen> CS_FORCE_INLINE
	bool isTailTypo(const Phrase<plen>& phrase, atimes_t atimes,
		const typename PhraseTrait<plen - 1>::MapType& prefixmap) const;

	template<uint8_t plen>
	void extractWords_(typename PhraseTrait<plen>::MapType& map,
			const typename PhraseTrait<plen - 1>::MapType& prefixmap,
			const typename PhraseTrait<plen - 1>::PadMap& padmap,
			const typename PhraseTrait<plen - 1>::PadMap& prxmap
		)
	{
		BOOST_STATIC_ASSERT(plen > 1);

		typedef Phrase<plen - 1> ShorterPhraseType;
		WordExamineRes res = no;
		for (typename PhraseTrait<plen>::MapType::const_iterator it = map.begin(); it != map.end(); ++it)
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
			const typename PhraseTrait<plen>::MapType& map,
			const typename PhraseTrait<plen - 1>::MapType& prefixmap,
			const typename PhraseTrait<plen - 1>::PadMap& padmap
		) const;

	template<uint8_t plen>
	Analyzer::WordExamineRes judgePrx(const Phrase<plen>& phrase,
			const typename PhraseTrait<plen>::MapType& map,
			const typename PhraseTrait<plen - 1>::MapType& prefixmap,
			const typename PhraseTrait<plen - 1>::PadMap& padmap
		) const;

	void caltureTotalAtimes()
	{
		#define _JEBE_CALL_TOTALATIMES(Z, n, N)		caltureTotalAtimes_<n>(BOOST_PP_CAT(map, n));
		BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_CALL_TOTALATIMES, BOOST_PP_EMPTY())
		#undef _JEBE_CALL_TOTALATIMES
	}

	template<uint8_t plen>
	void caltureTotalAtimes_(const typename PhraseTrait<plen>::MapType& map)
	{
		typedef typename PhraseTrait<plen>::MapType::const_iterator IterType;
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
	void buildPadMap_(typename PhraseTrait<prefix_len>::PadMap& padmap, const typename PhraseTrait<prefix_len + 1>::MapType& map)
	{
		typedef PhraseTrait<prefix_len> PrefixTrait;
		typedef typename PrefixTrait::PhraseType PrefixType;
		typedef typename PrefixTrait::SuffixType SuffixType;
		typedef typename PhraseTrait<prefix_len + 1>::MapType MapType;

		typedef typename PrefixTrait::PadList PadListType;
		typedef typename PrefixTrait::PadMap PadMapType;

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
				plist.append(typename PrefixTrait::PadType(suffix, it->second));
				CS_SAY("first " << prefix_len << ", prefix: [" << prefix << "], suffix: [" << suffix << "], atimes: " << it->second);
			}
			else
			{
				padit->second += it->second;
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
	void buildPrxMap_(typename PhraseTrait<prefix_len>::PadMap& prxmap, const typename PhraseTrait<prefix_len + 1>::MapType& map)
	{
		typedef PhraseTrait<prefix_len> SuffixTrait_;
		typedef typename SuffixTrait_::PhraseType SuffixType;
		typedef typename SuffixTrait_::SuffixType PrefixType;

		typedef PhraseTrait<prefix_len + 1> Trait;
		typedef typename Trait::MapType MapType;

		typedef typename SuffixTrait_::PadList PadListType;
		typedef typename SuffixTrait_::PadMap PadMapType;

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
				plist.append(typename SuffixTrait::PadType(prefix, it->second));
				CS_SAY("first " << prefix_len << ", prefix: [" << suffix << "], suffix: [" << prefix << "], atimes: " << it->second);
			}
			else
			{
				padit->second += it->second;
				CS_SAY("repeat " << prefix_len << ", prefix: [" << suffix << "], suffix: " << prefix << "], atimes: " << it->second);
			}
			CS_SAY("plist.sum: " << plist.sum << " (" << &plist << ")");
		}
	}

	void buildSuffixMap()
	{
		typedef PhraseTrait<1> Trait;
		typedef Trait::MapType MapType;
		for (MapType::const_iterator it = map1.begin(); it != map1.end(); ++it)
		{
			smap[it->first] = it->second;
		}
	}

	void clean(std::size_t min_atimes);

	template<uint8_t plen>
	void clean_(typename PhraseTrait<plen>::MapType& map, std::size_t min_atimes);
};

} /* namespace cws */
} /* namespace jebe */
