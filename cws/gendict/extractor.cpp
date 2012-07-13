
#include "extractor.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <cstdlib>
#include <boost/checked_delete.hpp>
#include <boost/iostreams/code_converter.hpp>
#include <boost/iostreams/device/file.hpp>
#include "urlcode.hpp"
#include "mbswcs.hpp"

namespace jebe {
namespace cws {

template<uint8_t plen>
Analyzer::WordExamineRes Analyzer::isWord(const Phrase<plen>& phrase,
		typename Phrase<plen>::MapType& map,
		typename Phrase<plen - 1>::MapType& prefixmap,
		typename Phrase<plen - 1>::PadMap& padmap
		) const
{
	typedef Phrase<plen - 1> PhraseType;
	typedef typename PhraseType::MapType MapType;
	typedef typename PhraseType::PadMap PadMapType;
	typedef typename PhraseType::PadList PadListType;
	typedef typename PhraseType::Suffix SuffixType;

	PhraseType prefix(phrase.str);
	SuffixType suffix(phrase.str + (plen - 1));

//		PadListType& plist = padmap[prefix];
	// both count(prefix) and  can not be 0, it is impossible.
	/*
	double joinprobActual = count(phrase) / count(prefix),
			joinprobPred = count(suffix) / totalAtimes;
	double overRate = joinprobActual / joinprobPred;
	// so, one a bit fast and explicit edition: */
	double atimes = map[phrase];
	double overRate = std::sqrt(atimes) * atimes / prefixmap[prefix] * totalAtimes / smap[suffix];

	if (overRate < joinThresholdLower)
	{
		return no;
	}

	PadListType& plist = padmap[prefix];
	if (plist->size() == 1)
	{
		return should_cover;
	}

	double entropy = 0., rate = 0.;
	for (typename PadListType::const_iterator it = plist->begin(); it != plist->end(); ++it)
	{
		entropy = 0;
		rate = static_cast<double>(it->second) / plist.sum;
		entropy -= rate * std::log(rate);
	}

	CS_SAY(phrase << " entropy: " << entropy);
	return entropy > entropyThreshold ? yes : should_cover;

	if (overRate > joinThresholdUpper)
	{
		CS_SAY("must be a word: [" << phrase.c_str() << "], overRate: " << overRate
			<< ", map[phrase]: " << map[phrase]
			<< ", prefixmap[prefix]: " << prefixmap[prefix]
			<< ", map1[suffix]: " << smap[suffix]
		);
		return should_cover;
	}

	if (overRate > joinThresholdLower)
	{
		CS_SAY("can be a word: [" << phrase.c_str() << "]");
	}

	CS_SAY("can be a word: [" << phrase.c_str() << "], overRate: " << overRate
		<< ", map[phrase]: " << map[phrase]
		<< ", prefixmap[prefix]: " << prefixmap[prefix]
		<< ", map1[suffix]: " << smap[suffix]
	);

	return yes;

//		std::log(joinfreq);
}

void Analyzer::analysis()
{
	clean(_JEBE_WORD_MIN_ATIMES);
	buildSuffixMap();
	buildPadMap();
	map6.clear();
	caltureTotalAtimes();
	extractWords();
}

void Analyzer::clean(std::size_t min_atimes)
{
	clean_<1>(map1, min_atimes);
	clean_<2>(map2, min_atimes);
	clean_<3>(map3, min_atimes);
	clean_<4>(map4, min_atimes);
	clean_<5>(map5, min_atimes);
	clean_<6>(map6, min_atimes);
	clean_<7>(map7, min_atimes);
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
	Analyzer azer(map1, map2, map3, map4, map5, map6, map7);
	azer.analysis();

	std::wofstream ofile(outfile.string().c_str(), std::ios_base::trunc);
	ofile.imbue(std::locale(""));
	Analyzer::Words& words = azer.getWords();
	for (Analyzer::Words::const_iterator it = words.begin(); it != words.end(); ++it)
	{
		ofile << it->c_str() << std::endl;
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
			if (!hasChs)
			{
				hasChs = true;
			}
		}
		else
		{
			if (hasChs)
			{
				CS_SAY("i: " << i  << ", chkPoint: " << chkPoint);
				addSentence_(str + chkPoint, i - chkPoint);

				hasChs = false;
			}
			chkPoint = i + 1;
		}
		++i;
	}

	if (hasChs)
	{
		CS_SAY("i: " << i  << ", chkPoint: " << chkPoint);
		addSentence_(str + chkPoint, i - chkPoint);
		hasChs = false;
	}
}

void Extractor::addSentence_(const CharType* const str, String::size_type len)
{
	scanSentence<1>(str, len, map1);
	scanSentence<2>(str, len, map2);
	scanSentence<3>(str, len, map3);
	scanSentence<4>(str, len, map4);
	scanSentence<5>(str, len, map5);
	scanSentence<6>(str, len, map6);
	scanSentence<7>(str, len, map7);
}

template<uint8_t plen>
void Extractor::scanSentence(const CharType* const str, String::size_type len,
		typename Phrase<plen>::MapType& map)
{
	if (CS_BUNLIKELY(len < plen))
	{
		return;
	}

	for (String::size_type i = 0, end = len - plen; i <= end; ++i)
	{
		typedef Phrase<plen> PhraseType;

		PhraseType p(str + i);
		typename Phrase<plen>::MapType::iterator it = map.find(p);

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
	for (Ph1::MapType::iterator it = map1.begin(); it != map1.end(); ++it)
	{
		CS_SAY("phrase1: [" << it->first.c_str() << "]: " << it->second);
	}

	for (Ph2::MapType::iterator it = map2.begin(); it != map2.end(); ++it)
	{
		CS_SAY("phrase2: [" << it->first.c_str() << "]: " << it->second);
	}

	for (Ph3::MapType::iterator it = map3.begin(); it != map3.end(); ++it)
	{
		CS_SAY("phrase3: [" << it->first.c_str() << "]: " << it->second);
	}

	for (Ph4::MapType::iterator it = map4.begin(); it != map4.end(); ++it)
	{
		CS_SAY("phrase4: [" << it->first.c_str() << "]: " << it->second);
	}

	for (Ph5::MapType::iterator it = map5.begin(); it != map5.end(); ++it)
	{
		CS_SAY("phrase5: [" << it->first.c_str() << "]: " << it->second);
	}

	for (Ph6::MapType::iterator it = map6.begin(); it != map6.end(); ++it)
	{
		CS_SAY("phrase6: [" << it->first.c_str() << "]: " << it->second);
	}

	for (Ph7::MapType::iterator it = map7.begin(); it != map7.end(); ++it)
	{
		CS_SAY("phrase7: [" << it->first.c_str() << "]: " << it->second);
	}
}

#define _JEBE_GB2312_CHAR_NUM 6763
Extractor::Extractor(const boost::filesystem::path& gbfile)
{
	CharType* gb = new CharType[_JEBE_GB2312_CHAR_NUM + 1];
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
	CharType* content = new CharType[_JEBE_PROCESS_STEP + 1];

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
/*
void fetchContent(const boost::filesystem::path& contentfile,
		const boost::filesystem::path& outfile, uint32_t maxchars)
{
	uint32_t processed = 0;
	std::size_t buf_remains = 0, last_buf_remains = 0;
	std::ifstream ifile(contentfile.string().c_str());

	const std::size_t
		bufsize = ((_JEBE_PROCESS_STEP) + 1) * sizeof(char),
		mbssize = ((_JEBE_PROCESS_STEP) * 2 + 1) * sizeof(char),
		wssize = (mbssize / sizeof(char)) * sizeof(wchar_t),
		memsize = wssize;

	CS_SAY("bufsize:" << bufsize << ", mbssize:" << mbssize << ", wssize:" << wssize << ",memsize:" << memsize);

	char* const buf = new char[bufsize / sizeof(char)];
	char* const mbs = new char[mbssize / sizeof(char)];
	wchar_t* const ws = new wchar_t[wssize / sizeof(wchar_t)];
	char* const mem = new char[memsize / sizeof(char)];

	memset(buf, 0, bufsize);
	memset(mbs, 0, mbssize);
	memset(ws, 0, wssize);

	std::size_t converted = 0, mbs_consumed = 0, mbs_len = 0;
	ssize_t readed = 0, mbs_remains = 0;

	try
	{
		while ((readed = ifile.readsome(buf + buf_remains, _JEBE_PROCESS_STEP - buf_remains)))
		{
			CS_STDOUT << std::endl << "--------------------------------------------------" << std::endl;
			CS_SAY("readed: " << readed << ", buf_remains: " << buf_remains);
			memset(mem, 0, memsize);
			processed += readed;
			if (maxchars != 0 && processed > maxchars)
			{
				break;
			}

			// ---------- urldecode
			last_buf_remains = buf_remains;
			CS_SAY("strlen(mbs): " << strlen(mbs));
			mbs_len = 0;
			buf_remains = staging::urldecode(buf, mbs + mbs_remains, readed + last_buf_remains, &mbs_len);
			CS_SAY("mbs:" << strlen(mbs) << ": [" << mbs << "]");
			CS_SAY("buf_remains: " << buf_remains);
			if (buf_remains)
			{
				memcpy(mem, buf + (readed + last_buf_remains - buf_remains), buf_remains);
				memset(buf, 0, bufsize);
				memcpy(buf, mem, buf_remains);
			}
			else
			{
				memset(buf, 0, bufsize);
			}

			// ---------- mbs => wcs
			memset(ws, 0, wssize);
			CS_SAY("mbs_remains: " << mbs_remains << ", mbs_len: " << mbs_len << ", strlen(mbs): " << strlen(mbs));
			size_t mbs_converted = 0;
			converted = staging::mbswcs::mb2wc(mbs, ws, mbs_len, &mbs_converted);

			mbs_remains = 0;
			if (converted == static_cast<size_t>(-1))
			{
				CS_SAY("convert failed, converted " << converted);
				// ----------- entirely clean ----------------
				//buf_remains = 0;
				mbs_remains = 0;
				mbs_remains = 0;


				memset(mem, 0, memsize);
				memcpy(mem, mbs + mbs_consumed, mbs_remains);
				memset(mbs, 0, mbssize);
				memcpy(mbs, mem, mbs_remains);
				memset(mem, 0, memsize);

				//memset(buf, 0, bufsize);
				memset(mbs, 0, mbssize);
				memset(ws, 0, wssize);
				memset(mem, 0, memsize);
			}
			else
			{
				if (converted > 0)
				{
#if CS_DEBUG > 1
					CS_SAY("ws: " << ws);
#endif
					scan(ws, converted);

					mbs_consumed = mbs_converted;
//					mbs_consumed = wcstombs(NULL, ws, 0);
					if (mbs_consumed == static_cast<size_t>(-1))
					{
						CS_DIE("mbs_consumed: " << mbs_consumed);
					}

					mbs_remains = strlen(mbs) - mbs_consumed;
					CS_SAY("converted: " << converted << ", mbs_consumed: " << mbs_consumed << ", mbs_remains: " << mbs_remains);

					if (mbs_remains > 0)
					{
						memset(mem, 0, memsize);
						memcpy(mem, mbs + mbs_consumed, mbs_remains);
						memset(mbs, 0, mbssize);
						memcpy(mbs, mem, mbs_remains);
						memset(mem, 0, memsize);
					}
					else if (mbs_remains == 0)
					{
						memset(mbs, 0, mbssize);
						memset(mem, 0, memsize);
					}
					else
					{
						CS_DIE("mbs_remains < 0, converted = " << converted << ", strlen(mbs) = " << strlen(mbs));
					}
				}
				else
				{
					mbs_remains = mbs_len;
				}
			}
		}
		ifile.close();
	}
	catch (std::exception& e)
	{
		CS_DIE("error occured while reading contentfile " << contentfile << ": " << e.what());
	}

//	delete[] buf;
//	delete[] mbs;
//	delete[] ws;
//	delete[] mem;
//	boost::checked_array_delete(buf);
//	boost::checked_array_delete(mbs);
//	boost::checked_array_delete(ws);
//	boost::checked_array_delete(mem);
}
*/
} /* namespace cws */
} /* namespace jebe */
