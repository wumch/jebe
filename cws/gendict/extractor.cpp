
#include "extractor.impl.hpp"
#include "extractor.hpp"
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
#include "urlcode.hpp"
#include "mbswcs.hpp"

namespace jebe {
namespace cws {
std::wstringstream log;
template<uint8_t plen>
Analyzer::WordExamineRes Analyzer::judge(const Phrase<plen>& phrase,
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

	log << phrase.c_str() << "\t";
	// both count(prefix) and count(suffix) can not be 0, it is impossible.
	/*
	double joinprobActual = count(phrase) / count(prefix),
			joinprobPred = count(suffix) / totalAtimes;
	double overRate = joinprobActual / joinprobPred;
	// so, the a bit fast and explicit edition: */
	const atimes_t
		atimes = map.find(phrase)->second,
		prefix_atimes = (plen == 2) ?  smap[prefix] : prefixmap.find(prefix)->second;
	const double overRate = std::sqrt(atimes) * atimes / prefix_atimes * totalAtimes / smap[suffix];
	log << "\tatimes/patimes:" << atimes << "," << prefix_atimes << "\toverRate: " << overRate;

	uint32_t res = (overRate > joinThresholdUpper) ? yes : no;
	if (CS_BLIKELY(overRate < joinThresholdLower))
	{
		return no;
	}

//	uint32_t res = (overRate > joinThresholdUpper) ? yes : no;
	if (CS_BUNLIKELY(prefix_atimes == atimes))
	{
		res |= yes | typo_prefix;
	}
//	else
//	{
		const PadListType& plist = padmap.find(prefix)->second;
		double entropy = 0., rate = 0.;
		for (typename PadListType::const_iterator it = plist->begin(); it != plist->end(); ++it)
		{
			rate = static_cast<double>(it->second) / plist.sum;
			entropy -= rate * std::log(rate);
		}
		log << "\trate:" << rate << ",entropy:" << entropy;
		CS_SAY(phrase << " entropy: " << entropy);
		if (CS_BUNLIKELY(entropy > entropyThresholdUpper))
		{
//			if (CS_BUNLIKELY(!(res & yes)))
//			{
				res |= no;
//			}
		}
		else if (CS_BUNLIKELY(entropy < entropyThresholdLower))
		{
			if (CS_BUNLIKELY(!(res & yes)))
			{
				res |= yes | typo_prefix;
			}
		}
//	}

	if (CS_LIKELY(res & yes))
	{
		if (isTailTypo(phrase, atimes, prefixmap))
		{
			res |= typo_suffix;
			log << "\tprefix-is-typo";
		}
	}
	log << "\tres: " << std::bitset<4>(res).to_string().c_str() << ((res & yes) ? "yes" : "not-yes") << std::endl;
	return static_cast<WordExamineRes>(res);
}

void Analyzer::analysis()
{
	clean(_JEBE_WORD_MIN_ATIMES);
	buildSuffixMap();
	buildPadMap();
	BOOST_PP_CAT(map, BOOST_PP_INC(_JEBE_WORD_MAX_LEN)).clear();
	caltureTotalAtimes();
	extractWords();
	CS_STDOUT << log.str() << std::endl;
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
	return atimes >= MinMiss<plen>::rate * shorterMap.find((Phrase<plen - 1>(phrase.str + 1)))->second;
}

template<> CS_FORCE_INLINE
bool Analyzer::isTailTypo<2>(const Phrase<2>& phrase, atimes_t atimes,
		const typename Phrase<1>::MapType& shorterMap) const
{
	return false;
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

void Extractor::extract(const boost::filesystem::path& outfile)
{
	#define _JEBE_AZER_ARG(Z, n, N)			BOOST_PP_CAT(map, n)BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
	std::auto_ptr<Analyzer> azer(new Analyzer(BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_AZER_ARG, BOOST_PP_EMPTY())));
	#undef _JEBE_AZER_ARG
	azer->analysis();

	std::wofstream ofile(outfile.string().c_str(), std::ios_base::trunc);
	ofile.imbue(std::locale(""));
	const Analyzer::Words& words = azer->getWords();
	for (Analyzer::Words::const_iterator it = words.begin(); it != words.end(); ++it)
	{
		ofile << it->c_str() << '\n';
	}
	ofile.close();
}

void Extractor::extract(const boost::filesystem::path& contentfile,
		const boost::filesystem::path& outfile, uint32_t maxchars)
{
	fetchContent(contentfile, outfile, maxchars);
	extract(outfile);
}

void Extractor::scan(const CharType* const str, String::size_type len)
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

void Extractor::addSentence(const CharType* const str, String::size_type len)
{
	#define _JEBE_CALL_SCAN(Z, n, N)		scanSentence_<n>(str, len, BOOST_PP_CAT(map, n));
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_CALL_SCAN, BOOST_PP_EMPTY())
	#undef _JEBE_CALL_SCAN
}

template<uint8_t plen>
void Extractor::scanSentence_(const CharType* const str, String::size_type len,
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
		CS_SAY("phrase1: [" << it->first.c_str() << "]: " << it->second);					\
	}
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DO_DISPLAY, BOOST_PP_EMPTY())
	#undef _JEBE_CALL_SCAN
}

#define _JEBE_GB2312_CHAR_NUM 6763
#define _JEBE_EXTRACTOR_INIT(Z, n, N)		BOOST_PP_CAT(map, n)BOOST_PP_LPAREN()MapHashBits<n>::bits BOOST_PP_RPAREN()BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
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

void Extractor::fetchContent(const boost::filesystem::path& contentfile,
		const boost::filesystem::path& outfile, uint32_t maxchars)
{
	CharType* const content = new CharType[_JEBE_PROCESS_STEP + 1];

	std::wfstream file(contentfile.string().c_str(), std::ios_base::in);
	CS_SAY("imbue");
	file.imbue(std::locale(""));
	CS_SAY("read");

	uint32_t processed = 0;
	ssize_t readed = 0;
	while (true)
	{
		memset(content, 0, _JEBE_PROCESS_STEP + 1);
		CS_SAY("content readed: " << readed);
		if (CS_BUNLIKELY((readed = file.readsome(content, _JEBE_PROCESS_STEP)) <= 0))
		{
			break;
		}
		if (CS_BUNLIKELY(maxchars != 0))
		{
			processed += readed;
			if (CS_BUNLIKELY(processed > maxchars))
			{
				break;
			}
		}
		scan(content, readed);
	}
	delete[] content;
	file.close();
}

} /* namespace cws */
} /* namespace jebe */
