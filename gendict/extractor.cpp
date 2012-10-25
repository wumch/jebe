
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
#include "urlcode.hpp"
#include "mbswcs.hpp"
#include "memory.hpp"
#include "utils.hpp"
#include "phrasetrait.hpp"
#include "analyzer.hpp"

namespace jebe {
namespace cws {

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
	ofile << words_count << '\t' << words_atimes << CS_LINESEP;

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
		ofile << it->first.c_str() << '\t' << it->second << CS_LINESEP;
	}
	ofile.close();
}

void Extractor::extract(const PathList& contentfiles,
		const boost::filesystem::path& outfile)
{
	scan(contentfiles);
	dump(outfile);
}

void Extractor::scanPara(CharType* const str, String::size_type len)
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
				CS_SAY("i: " << i << ", chkPoint: " << chkPoint);
				addSentence(str + chkPoint, i - chkPoint);
				hasChs = false;
			}
		}
		++i;
	}

	if (hasChs)
	{
		CS_SAY("i: " << i << ", chkPoint: " << chkPoint);
		addSentence(str + chkPoint, i - chkPoint);
		hasChs = false;
	}
}

void Extractor::addSentence(CharType* const str, String::size_type len)
{
//	scanPrepends(str, len);
//	if (CS_BUNLIKELY(scanLatin(str, len)))
//	{
//		return;
//	}
	#define _JEBE_CALL_SCAN(Z, n, N)		scanSentence_<n>(str, len, BOOST_PP_CAT(map, n));
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_CALL_SCAN, BOOST_PP_EMPTY())
	#undef _JEBE_CALL_SCAN
}

template<uint8_t plen>
void Extractor::scanSentence_(CharType* const str, String::size_type len,
	typename PhraseTrait<plen>::MapType& map)
{
	if (CS_BUNLIKELY(len < plen))
	{
		return;
	}

	typedef PhraseTrait<plen> PhraseTrait;
	for (String::size_type i = 0, end = len - plen; i <= end; ++i)
	{
		const typename PhraseTrait::PhraseType p(str + i);
		typename PhraseTrait::MapType::iterator it = map.find(p);

		if (plen == 1)
		{
			replacetolower(str + i);
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

void Extractor::scanPrepends(CharType* const str, String::size_type len)
{

}

// return true iff all are latin characters.
bool Extractor::scanLatin(CharType* const str, String::size_type len)
{
	bool allarelatin = true;
	for (uint i = 0, chkpoint = 0, latins = 0; i < len; ++i)
	{
		if (CS_BUNLIKELY(isAsciiStrict(str[i])))
		{
			latins += 1;
		}
		else
		{
			if (_JEBE_LATIN_MIN_LEN <= latins && latins <= _JEBE_LATIN_MAX_LEN)
			{
				LatinPhrase word;
				word.assign(str + chkpoint, str + chkpoint + latins);
				LatinPhraseMapType::iterator it = latinp.find(word);
				if (it == latinp.end())
				{
					latinp.insert(std::make_pair(word, 1));
					CS_SAY("latin-word: " << String(str + chkpoint, latins));
				}
				else
				{
					it->second += 1;
				}
			}
			latins = 0;
			chkpoint = i + 1;
			allarelatin = false;
		}
	}
	return allarelatin;
}

void Extractor::display()
{
	#define _JEBE_DO_DISPLAY(Z, n, N)														\
	for (PhraseTrait<n>::MapType::iterator it = BOOST_PP_CAT(map, n).begin(); 				\
		it != BOOST_PP_CAT(map, n).end(); ++it)												\
	{																						\
		CS_SAY("phrase" << n << ": [" << it->first << "]: " << it->second);					\
	}
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_DO_DISPLAY, BOOST_PP_EMPTY())
	#undef _JEBE_CALL_SCAN
}

#define _JEBE_GB2312_CHAR_NUM (6763+63)
#define _JEBE_EXTRACTOR_INIT(Z, n, N)		BOOST_PP_CAT(map, n)BOOST_PP_LPAREN()(1 << MapHashBits<n>::bits)BOOST_PP_RPAREN()BOOST_PP_COMMA_IF(BOOST_PP_LESS_EQUAL(n, _JEBE_WORD_MAX_LEN))
Extractor::Extractor(const boost::filesystem::path& gbfile)
	: BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2), _JEBE_EXTRACTOR_INIT, BOOST_PP_EMPTY())
{
	CharType* const gb = new CharType[_JEBE_GB2312_CHAR_NUM + 1];
	memset(gb, 0, (_JEBE_GB2312_CHAR_NUM + 1) * sizeof(CharType));

	std::wifstream ifile(gbfile.string().c_str());
	ifile.imbue(std::locale(""));

	CS_SAY("starting reading gb2312-file");
	ssize_t readed = 0;
	while (static_cast<int64_t>(ifile.peek()) != EOF && ifile.good())
	{
		readed += ifile.readsome(gb + readed, _JEBE_GB2312_CHAR_NUM - readed);
		CS_SAY("gb2312-file readed:" << readed);
	}
	CS_SAY("finally, gb2312-file readed:" << readed);

	for (uint16_t i = 0; i < _JEBE_GB2312_CHAR_NUM; ++i)
	{
		if (CS_BLIKELY(gb[i] < _JEBE_GB_CHAR_MAX))
		{
			gb2312[gb[i]] = true;
		}
	}

	delete[] gb;
}

void Extractor::scan(const PathList& contentfiles)
{
	CharType* const content = new CharType[_JEBE_PROCESS_STEP + 1];

	for (PathList::const_iterator it = contentfiles.begin(); it != contentfiles.end(); ++it)
	{
		std::wfstream file(it->string().c_str(), std::ios_base::in);
		file.imbue(std::locale(""));

		ssize_t cur = 0;
		while (file.good() && static_cast<int64_t>(file.peek()) != EOF)
		{
			memset(content, 0, _JEBE_PROCESS_STEP + 1);
			file.getline(content, _JEBE_PROCESS_STEP);
			CS_SAY("content readed: " << static_cast<std::streamoff>(file.tellg()));
			scanPara(content, static_cast<std::streamoff>(file.tellg()) - cur);
			cur = static_cast<std::streamoff>(file.tellg());
		}
		file.close();
		std::cout << it->string() << " done" << std::endl;
	}
	delete[] content;
}

} /* namespace cws */
} /* namespace jebe */
