
#include "analyzer.hpp"

namespace jebe {
namespace cws {

template<uint8_t plen>
Analyzer::WordExamineRes Analyzer::judgePad(const Phrase<plen>& phrase,
	const typename Phrase<plen>::MapType& map,
	const typename Phrase<plen - 1>::MapType& prefixmap,
	const typename Phrase<plen - 1>::PadMap& padmap
) const
{
	typedef Phrase<plen - 1> PhraseType;
	typedef typename PhraseType::MapType MapType;
	typedef typename PhraseType::PadMap PadMapType;
	typedef typename PhraseType::PadList PadListType;
	typedef typename PhraseType::Suffix SuffixType;

	const PhraseType prefix(phrase.str);
	const SuffixType suffix(phrase.str + (plen - 1));

	CS_LOG(phrase.c_str() << "\t");
	// neither count(prefix) nor count(suffix) won't be 0 --- it is impossible.
	/*
	double joinprobActual = count(phrase) / count(prefix),
			joinprobPred = count(suffix) / totalAtimes;
	double overRate = joinprobActual / joinprobPred;
	// so, the a bit fast and explicit edition: */
	const atimes_t
		atimes = map.find(phrase)->second,
		prefix_atimes = (plen == 2) ?  smap[prefix] : prefixmap.find(prefix)->second;
	const double overRate = static_cast<double>(atimes) / prefix_atimes * totalAtimes[SuffixType::len - 1] / smap[suffix];
	CS_LOG("\tatimes/patimes:" << atimes << "," << prefix_atimes << "\toverRate: " << overRate);

	if (CS_BLIKELY(overRate < joinThresholdLower))
	{
		CS_LOG(std::endl);
		return no;
	}

	uint32_t res;
	if (CS_BUNLIKELY(overRate > joinThresholdUpper))
	{
		res = yes;
	}

	const PadListType& plist = padmap.find(prefix)->second;
	double entropy = 0., rate = 0.;
	for (typename PadListType::const_iterator it = plist->begin(); it != plist->end(); ++it)
	{
		rate = static_cast<double>(it->second) / plist.sum;
		entropy -= rate * std::log(rate);
	}
	CS_LOG("\trate:" << rate << ",entropy:" << entropy);
	CS_SAY(phrase.c_str() << " entropy: " << entropy);
	if (CS_BUNLIKELY(entropy > entropyThresholdUpper))
	{
		res |= no;
	}
	else
	{
		res |=  CS_BLIKELY(atimes > atimesThreshold) ? yes : no;
		if (CS_BUNLIKELY(entropy < entropyThresholdLower))
		{
			res |= typo_prefix;
		}
	}

	CS_LOG("\tres: " << std::bitset<4>(res).to_string().c_str() << ((res & yes) ? "yes" : "not-yes") << std::endl);
	return static_cast<WordExamineRes>(res);
}

template<uint8_t plen>
Analyzer::WordExamineRes Analyzer::judgePrx(const Phrase<plen>& phrase,
	const typename Phrase<plen>::MapType& map,
	const typename Phrase<plen - 1>::MapType& suffixmap,
	const typename Phrase<plen - 1>::PadMap& prxmap
) const
{
	typedef Phrase<plen - 1> PhraseType;
	typedef typename PhraseType::MapType MapType;
	typedef typename PhraseType::PadMap PadMapType;
	typedef typename PhraseType::PadList PadListType;
	typedef typename PhraseType::Suffix SuffixType;

	const PhraseType suffix(phrase.str + 1);
	const SuffixType prefix(phrase.str);

	CS_LOG(phrase.c_str() << "\t");
	// both count(prefix) and count(suffix) can not be 0, it is impossible.
	/*
	double joinprobActual = count(phrase) / count(prefix),
			joinprobPred = count(suffix) / totalAtimes;
	double overRate = joinprobActual / joinprobPred;
	// so, the a bit fast and explicit edition: */
	const atimes_t
		atimes = map.find(phrase)->second,
		suffix_atimes = (plen == 2) ?  smap[suffix] : suffixmap.find(suffix)->second;
	const double overRate = static_cast<double>(atimes) / suffix_atimes * totalAtimes[PhraseType::len - 1] / smap[prefix];
	CS_LOG("\tatimes/patimes:" << atimes << "," << suffix_atimes << "\toverRate: " << overRate);

	if (CS_BLIKELY(overRate < joinThresholdLower))
	{
		CS_LOG(std::endl);
		return no;
	}

	uint32_t res;
	if (CS_BUNLIKELY(overRate > joinThresholdUpper))
	{
		res = yes;
	}

	const PadListType& plist = prxmap.find(suffix)->second;
	double entropy = 0., rate = 0.;
	for (typename PadListType::const_iterator it = plist->begin(); it != plist->end(); ++it)
	{
		rate = static_cast<double>(it->second) / plist.sum;
		entropy -= rate * std::log(rate);
	}
	CS_LOG("\trate:" << rate << ",entropy:" << entropy);
	CS_SAY(phrase.c_str() << " entropy: " << entropy);
	if (CS_BUNLIKELY(entropy > entropyThresholdUpper))
	{
		res |= no;
	}
	else
	{
		res |= CS_BLIKELY(atimes > atimesThreshold) ? yes : no;
		if (CS_BUNLIKELY(entropy < entropyThresholdLower))
		{
			res |= typo_suffix;
		}
	}

	CS_LOG("\tres: " << std::bitset<4>(res).to_string().c_str() << ((res & yes) ? "yes" : "not-yes") << std::endl);
	return static_cast<WordExamineRes>(res);
}

void Analyzer::analysis()
{
	caltureTotalAtimes();
	clean(_JEBE_WORD_MIN_ATIMES);
	buildSuffixMap();
	buildPadMap();
	buildPrxMap();
	BOOST_PP_CAT(map, BOOST_PP_INC(_JEBE_WORD_MAX_LEN)).clear();
	extractWords();
#if CS_LOG_ON
	CS_STDOUT << log.str() << std::endl;
#endif
}

template<uint8_t plen> class MinMiss { public: static const double rate = 0.9; };
template<> class MinMiss<3> { public: static const double rate = 0.50; };
template<> class MinMiss<4> { public: static const double rate = 0.60; };
template<> class MinMiss<5> { public: static const double rate = 0.70; };
template<> class MinMiss<6> { public: static const double rate = 0.80; };
template<> class MinMiss<7> { public: static const double rate = 0.90; };
template<> class MinMiss<8> { public: static const double rate = 0.95; };

template<uint8_t plen> CS_FORCE_INLINE
bool Analyzer::isTailTypo(const Phrase<plen>& phrase, atimes_t atimes,
		const typename Phrase<plen - 1>::MapType& shorterMap) const
{
	if (plen == 2) return false;
	return atimes >= MinMiss<plen>::rate * shorterMap.find((Phrase<plen - 1>(phrase.str + 1)))->second;
}

void Analyzer::clean(std::size_t min_atimes)
{
	#define _JEBE_CALL_CLEAN(Z, n, N)		clean_<n>(BOOST_PP_CAT(map, n), min_atimes);
	BOOST_PP_REPEAT_FROM_TO(2, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_CALL_CLEAN, BOOST_PP_EMPTY())
	#undef _JEBE_CALL_CLEAN
}

template<uint8_t plen>
void Analyzer::clean_(typename Phrase<plen>::MapType& map, std::size_t min_atimes)
{
	typedef typename Phrase<plen>::MapType MapType;
	for (typename MapType::iterator it = map.begin(); it != map.end(); )
	{
		if (it->second < min_atimes)
		{
			it = map.erase(it);
		}
		else
		{
			++it;
		}
	}
}

} /* namespace cws */
} /* namespace jebe */
