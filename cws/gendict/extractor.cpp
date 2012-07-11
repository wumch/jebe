
#include "extractor.hpp"
#include <fstream>
#include <exception>
#include <cstdlib>
#include "urlcode.hpp"
#include "mbswcs.hpp"

namespace jebe {
namespace cws {

void Extractor::extract()
{
	Analyzer azer(ph2map, ph3map, ph4map, ph5map);
	azer.analysis();
}

#define _PROCESS_STEP (4 << 20)
void Extractor::extract(const boost::filesystem::path& file, uint32_t max_chars)
{
	uint32_t processed = 0;
	std::size_t buf_remains = 0, last_buf_remains = 0;
	std::ifstream ifile(file.c_str());

	const std::size_t
		bufsize = _PROCESS_STEP * sizeof(char),
		mbssize = (_PROCESS_STEP << 2) * sizeof(char),
		wssize = (_PROCESS_STEP << 2) * sizeof(wchar_t),
		memsize = (_PROCESS_STEP << 2) * sizeof(wchar_t);

	char* const buf = new char[bufsize];
	char* const mbs = new char[mbssize];
	wchar_t* const ws = new wchar_t[wssize];

	char* const mem = new char[memsize];
	memset(buf, 0, _PROCESS_STEP * sizeof(char));

	std::size_t converted = 0, mbs_consumed = 0;
	ssize_t readed = 0, mbs_remains = 0;

	try
	{
		while ((readed = ifile.readsome(buf + buf_remains, _PROCESS_STEP - buf_remains)))
		{
			processed += readed;
			if (max_chars != 0 && processed > max_chars)
			{
				break;
			}

			last_buf_remains = buf_remains;
			buf_remains = staging::urldecode(buf, mbs + mbs_remains, readed + last_buf_remains);
//			std::wcout << "buf_remains: " << buf_remains << std::endl;
			if (buf_remains)
			{
				std::memcpy(mem, buf + (readed + last_buf_remains - buf_remains), buf_remains);
				std::memset(buf, 0, bufsize);
				std::memcpy(buf, mem, buf_remains);
			}
			else
			{
				std::memset(buf, 0, bufsize);
			}

			memset(ws, 0, wssize);
			converted = staging::mbswcs::mb2wc(mbs, ws);

			if (converted == static_cast<size_t>(-1))
			{
//				std::wcout << "convert failed, converted " << converted << std::endl;
				converted = 0;
				mbs_remains = strlen(mbs);
			}
			else
			{
				if (converted > 0)
				{
#if CS_DEBUG > 1
					std::wcout << ws;
#endif
					scan(ws, converted);

					mbs_consumed = wcstombs(NULL, ws, 0);
					if (mbs_consumed == static_cast<size_t>(-1))
					{
						CS_DIE("mbs_consumed: " << mbs_consumed);
					}

					mbs_remains = strlen(mbs) - mbs_consumed;
//					std::wcout << "converted: " << converted << ", mbs_consumed: " << mbs_consumed << ", mbs_remains: " << mbs_remains << std::endl;

					if (mbs_remains > 0)
					{
						std::memset(mem, 0, memsize);
						std::memcpy(mem, mbs + mbs_consumed, mbs_remains);
						std::memset(mbs, 0, mbssize);
						std::memcpy(mbs, mem, mbs_remains);
						std::memset(mem, 0, memsize);
					}
					else if (mbs_remains == 0)
					{
						std::memset(mbs, 0, mbssize);
						std::memset(mem, 0, memsize);
					}
					else
					{
						CS_DIE("mbs_remains < 0, converted = " << converted << ", strlen(mbs) = " << strlen(mbs));
					}
				}
			}
		}
		ifile.close();
	}
	catch (std::exception& e)
	{
		CS_DIE("error occured while reading file " << file << ": " << e.what());
	}

	extract();
}

void Extractor::scan(const CharType* const str, String::size_type len)
{
	std::wcout << str << std::endl;
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
//				std::wcout << "i: " << i  << ", chkPoint: " << chkPoint << std::endl;
				scanSentence(str + chkPoint, i - chkPoint, ph2map);
				scanSentence(str + chkPoint, i - chkPoint, ph3map);
				scanSentence(str + chkPoint, i - chkPoint, ph4map);
				scanSentence(str + chkPoint, i - chkPoint, ph5map);

				hasChs = false;
			}
			chkPoint = i + 1;
		}
		++i;
	}

	if (hasChs)
	{
//		std::wcout << "i: " << i  << ", chkPoint: " << chkPoint << std::endl;
		scanSentence(str + chkPoint, i - chkPoint, ph2map);
		scanSentence(str + chkPoint, i - chkPoint, ph3map);
		scanSentence(str + chkPoint, i - chkPoint, ph4map);
		scanSentence(str + chkPoint, i - chkPoint, ph5map);

		hasChs = false;
	}
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
	for (Ph2Map::iterator it = ph2map.begin(); it != ph2map.end(); ++it)
	{
		std::wcout << "phrase2: [" << it->first.c_str() << "]: " << it->second << std::endl;
	}

	for (Ph3Map::iterator it = ph3map.begin(); it != ph3map.end(); ++it)
	{
		std::wcout << "phrase3: [" << it->first.c_str() << "]: " << it->second << std::endl;
	}

	for (Ph4Map::iterator it = ph4map.begin(); it != ph4map.end(); ++it)
	{
		std::wcout << "phrase4: [" << it->first.c_str() << "]: " << it->second << std::endl;
	}

	for (Ph5Map::iterator it = ph5map.begin(); it != ph5map.end(); ++it)
	{
		std::wcout << "phrase5: [" << it->first.c_str() << "]: " << it->second  << std::endl;
	}
}

#define _GB2312_CHAR_NUM 6763
Extractor::Extractor(const boost::filesystem::path& gbfile)
{
	std::ifstream ifile(gbfile.c_str(), std::ios_base::in | std::ios_base::binary);
	char* mbgb = new char[_GB2312_CHAR_NUM * 3 + 1];
	CharType* gb = new CharType[_GB2312_CHAR_NUM + 1];
	std::size_t len = ifile.readsome(mbgb, _GB2312_CHAR_NUM * 3);
	staging::mbswcs::mb2wc(mbgb, gb);
	std::wcout << len << std::endl;
	for (uint16_t i = 0; i < _GB2312_CHAR_NUM; ++i)
	{
		gb2312[gb[i]] = true;
	}
	delete [] mbgb;
	delete [] gb;
}

Extractor::~Extractor() {
	// TODO Auto-generated destructor stub
}

} /* namespace cws */
} /* namespace jebe */
