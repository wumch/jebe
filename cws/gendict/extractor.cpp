
#include "extractor.hpp"
#include "memory.hpp"
#include "extractor.impl.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <cstdlib>
#include <cmath>
#include <boost/iostreams/code_converter.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/preprocessor.hpp>
#include <boost/mpl/map.hpp>
#include <boost/pool/pool_alloc.hpp>
#include "urlcode.hpp"
#include "mbswcs.hpp"

namespace jebe {
namespace cws {

#if CS_LOG_ON
std::wstringstream log;
#define CS_LOG(...)  log << __VA_ARGS__
#else
#define CS_LOG(...)
#endif

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

void Extractor::dump(const boost::filesystem::path& outfile)
{
	#define _JEBE_AZER_ARG(Z, n, N)			BOOST_PP_CAT(map, n)BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
	std::auto_ptr<Analyzer> azer(new Analyzer(BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_AZER_ARG, BOOST_PP_EMPTY())));
	#undef _JEBE_AZER_ARG
	azer->analysis();

	std::wofstream ofile(outfile.string().c_str(), std::ios_base::trunc);
	ofile.imbue(std::locale(""));
	const Analyzer::Words& words = azer->getWords();

	std::size_t words_count = 0, words_atimes = 0;
	for (Analyzer::Words::const_iterator it = words.begin(); it != words.end(); ++it)
	{
		if (it->first.size() == 2)
		{
			if (CS_BUNLIKELY(isAscii(it->first[0])))
			{
				if (CS_BUNLIKELY(isAscii(it->first[1])))
				{
					continue;
				}
			}
		}
		++words_count;
		words_atimes += it->second;
	}
	ofile << words_count << "\t" << words_atimes << '\n';

	for (Analyzer::Words::const_iterator it = words.begin(); it != words.end(); ++it)
	{
		if (it->first.size() == 2)
		{
			if (CS_BUNLIKELY(isAscii(it->first[0])))
			{
				if (CS_BUNLIKELY(isAscii(it->first[1])))
				{
					continue;
				}
			}
		}
		ofile << it->first.c_str() << "\t" << it->second << '\n';
	}
	ofile.close();
}

void Extractor::extract(const PathList& contentfiles,
		const boost::filesystem::path& outfile)
{
	fetchContent(contentfiles);
	dump(outfile);
}

void Extractor::scan(CharType* const str, String::size_type len)
{
	CS_SAY(str);
	String::size_type i = 0, chkPoint = 0;
	bool hasChs = false;
	while (i < len)
	{
		if (CS_BLIKELY(isGb2312(str[i])))
		{
			if (CS_BUNLIKELY(!hasChs))
			{
				hasChs = true;
				chkPoint = i;
			}
		}
		else
		{
			if (CS_BLIKELY(hasChs))
			{
				CS_SAY("i: " << i  << ", chkPoint: " << chkPoint);
				addSentence(str + chkPoint, i - chkPoint);
				hasChs = false;
			}
		}
		++i;
	}

	if (hasChs)
	{
		CS_SAY("i: " << i  << ", chkPoint: " << chkPoint);
		addSentence(str + chkPoint, i - chkPoint);
		hasChs = false;
	}
}

void Extractor::addSentence(CharType* const str, String::size_type len)
{
	#define _JEBE_CALL_SCAN(Z, n, N)		scanSentence_<n>(str, len, BOOST_PP_CAT(map, n));
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_CALL_SCAN, BOOST_PP_EMPTY())
	#undef _JEBE_CALL_SCAN
}

template<uint8_t plen>
void Extractor::scanSentence_(CharType* const str, String::size_type len,
		typename Phrase<plen>::MapType& map)
{
	if (CS_BUNLIKELY(len < plen))
	{
		return;
	}

	for (String::size_type i = 0, end = len - plen; i <= end; ++i)
	{
		typedef Phrase<plen> PhraseType;

		const PhraseType p(str + i);
		typename PhraseType::MapType::iterator it = map.find(p);

		if (plen == 1)
		{
			if (CS_BUNLIKELY(L'A' <= str[i] && str[i] <= L'Z'))
			{
				str[i] += 32;
			}
		}

		if (CS_BLIKELY(it != map.end()))
		{
			it->second++;
		}
		else
		{
			map[p] = 1;
		}
	}
}

void Extractor::display()
{
	#define _JEBE_DO_DISPLAY(Z, n, N)														\
	for (BOOST_PP_CAT(Ph, n)::MapType::iterator it = BOOST_PP_CAT(map, n).begin(); 			\
		it != BOOST_PP_CAT(map, n).end(); ++it)												\
	{																						\
		CS_SAY("phrase" << n << ": [" << it->first << "]: " << it->second);			\
	}
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DO_DISPLAY, BOOST_PP_EMPTY())
	#undef _JEBE_CALL_SCAN
}

#define _JEBE_GB2312_CHAR_NUM (6763+63)
#define _JEBE_EXTRACTOR_INIT(Z, n, N)		BOOST_PP_CAT(map, n)BOOST_PP_LPAREN()1 << MapHashBits<n>::bits BOOST_PP_RPAREN()BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
Extractor::Extractor(const boost::filesystem::path& gbfile)
	: BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_EXTRACTOR_INIT, BOOST_PP_EMPTY())
{
	CharType* const gb = new CharType[_JEBE_GB2312_CHAR_NUM + 1];
	memset(gb, 0, (_JEBE_GB2312_CHAR_NUM + 1) * sizeof(CharType));

	std::wifstream ifile(gbfile.string().c_str());
	ifile.imbue(std::locale(""));

	ssize_t readed = 0;
	while (readed < _JEBE_GB2312_CHAR_NUM)
	{
		readed += ifile.readsome(gb + readed, _JEBE_GB2312_CHAR_NUM - readed);
	}
	CS_SAY("gb2312-file readed:" << readed);

	for (uint16_t i = 0; i < _JEBE_GB2312_CHAR_NUM; ++i)
	{
		assert(gb[i] < gb_char_max);
		gb2312[gb[i]] = true;
	}

	delete[] gb;
}

void Extractor::fetchContent(const PathList& contentfiles)
{
	CharType* const content = new CharType[_JEBE_PROCESS_STEP + 1];

	for (PathList::const_iterator it = contentfiles.begin(); it != contentfiles.end(); ++it)
	{
		std::wfstream file(it->string().c_str(), std::ios_base::in);
		CS_SAY("imbue");
		file.imbue(std::locale(""));
		CS_SAY("read");

//		uint32_t processed = 0;
		ssize_t readed = 0;
		while (true)
		{
			memset(content, 0, _JEBE_PROCESS_STEP + 1);
			CS_SAY("content readed: " << readed);
			if (CS_BUNLIKELY((readed = file.readsome(content, _JEBE_PROCESS_STEP)) <= 0))
			{
				break;
			}
//			if (CS_BUNLIKELY(maxchars != 0))
//			{
//				processed += readed;
//				if (CS_BUNLIKELY(processed > maxchars))
//				{
//					break;
//				}
//			}
			scan(content, readed);
		}
		file.close();
		std::cout << it->string() << " done" << std::endl;
	}
	delete[] content;
}

} /* namespace cws */
} /* namespace jebe */
