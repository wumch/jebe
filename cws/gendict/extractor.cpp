
#include "extractor.hpp"
#include <fstream>
#include <exception>
#include <cstdlib>
#include "urlcode.hpp"
#include "mbswcs.hpp"

namespace jebe {
namespace cws {

void Analyzer::analysis()
{
	clean(3);
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
}

template<uint8_t plen>
void Analyzer::clean_(typename Phrase<plen>::MapType& map, std::size_t min_atimes)
{
	typedef typename Phrase<plen>::MapType MapType;
	for (typename MapType::iterator it = map.begin(); it != map.end(); ++it)
	{
		if (it->second < min_atimes)
		{
			map.erase(it);
		}
	}
}

void Extractor::extract()
{
	Analyzer azer(map1, map2, map3, map4, map5, map6);
	azer.analysis();
}

#define _PROCESS_STEP (22)
void Extractor::extract(const boost::filesystem::path& file, uint32_t max_chars)
{
	uint32_t processed = 0;
	std::size_t buf_remains = 0, last_buf_remains = 0;
	std::ifstream ifile(file.string().c_str());

	const std::size_t
		bufsize = (_PROCESS_STEP + 1) * sizeof(char),
		mbssize = ((_PROCESS_STEP << 2) + 1) * sizeof(char),
		wssize = ((_PROCESS_STEP << 2) + 1) * sizeof(wchar_t),
		memsize = ((_PROCESS_STEP << 2) + 1) * sizeof(wchar_t);

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
		while ((readed = ifile.readsome(buf + buf_remains, _PROCESS_STEP - buf_remains)))
		{
			memset(mem, 0, memsize);
			processed += readed;
			if (max_chars != 0 && processed > max_chars)
			{
				break;
			}

			last_buf_remains = buf_remains;
			CS_SAY("strlen(mbs): " << strlen(mbs));
			buf_remains = staging::urldecode(buf, mbs + mbs_remains, readed + last_buf_remains, &mbs_len);
			CS_SAY("strlen(mbs): " << strlen(mbs));
//			CS_SAY(mbs);
			CS_SAY("[" << mbs << "]");
			CS_SAY("buf_remains: " << buf_remains << std::endl);
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

			memset(ws, 0, wssize);
			CS_SAY("mbs_len: " << mbs_len << ", strlen(mbs): " << strlen(mbs));
			converted = staging::mbswcs::mb2wc(mbs, ws, mbs_len - 1);
			mbs_len = 0;

			if (converted == static_cast<size_t>(-1))
			{
				CS_SAY("convert failed, converted " << converted);
				converted = 0;
				mbs_remains = strlen(mbs);
			}
			else
			{
				if (converted > 0)
				{
#if CS_DEBUG > 1
					CS_SAY("ws: " << ws);
#endif
					scan(ws, converted);

					mbs_consumed = wcstombs(NULL, ws, 0);
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
					mbs_remains = strlen(mbs);
				}
			}
		}
		ifile.close();
	}
	catch (std::exception& e)
	{
		CS_DIE("error occured while reading file " << file << ": " << e.what());
	}

	delete[] buf;
	delete[] mbs;
	delete[] ws;
	delete[] mem;

	extract();
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
	scanSentence(str, len, map1);
	scanSentence(str, len, map2);
	scanSentence(str, len, map3);
	scanSentence(str, len, map4);
	scanSentence(str, len, map5);
	scanSentence(str, len, map6);
}

template<uint8_t plen>
void Extractor::scanSentence(const CharType* const str, String::size_type len,
		boost::unordered_map<Phrase<plen>, atimes_t, PhraseHash<plen, _JEBE_BUCKET_BITS> >& phmap)
{
	if (len < plen)
	{
		return;
	}
	typedef Phrase<plen> Ph;
	for (String::size_type i = 0, end = len - plen; i <= end; ++i)
	{
		Ph ph(str + i);
		if (CS_BUNLIKELY(phmap.find(ph) == phmap.end()))
		{
			phmap[ph] = 1;
		}
		else
		{
			++phmap[ph];
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
}

#define _GB2312_CHAR_NUM 6763
Extractor::Extractor(const boost::filesystem::path& gbfile)
{
	std::ifstream ifile(gbfile.string().c_str(), std::ios_base::in | std::ios_base::binary);
	char* mbgb = new char[_GB2312_CHAR_NUM * 3 + 1];
	CharType* gb = new CharType[_GB2312_CHAR_NUM + 1];
	std::size_t len = ifile.readsome(mbgb, _GB2312_CHAR_NUM * 3);
	staging::mbswcs::mb2wc(mbgb, gb, len);
	CS_SAY(len);
	for (uint16_t i = 0; i < _GB2312_CHAR_NUM; ++i)
	{
		gb2312[gb[i]] = true;
	}
	delete[] mbgb;
	delete[] gb;
}

Extractor::~Extractor() {
	// TODO Auto-generated destructor stub
}

} /* namespace cws */
} /* namespace jebe */
