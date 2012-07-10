
#include "extractor.hpp"
#include <fstream>
#include <exception>
#include "../../staging/urlcode.hpp"

namespace jebe {
namespace cws {

void Extractor::extract(const boost::filesystem::path& file, uint32_t max_chars)
{
	uint32_t processed = 0;
	std::ifstream ifile(file.c_str());
	std::auto_ptr<std::stringbuf> buf(new std::stringbuf());
	try
	{
		while (ifile.get(*buf))
		{
			processed += buf->str().size();
			if (max_chars != 0 && processed > max_chars)
			{
				break;
			}
			std::string s = staging::URLDecode(buf->str());
			CS_SAY(s);
		}
		ifile.close();
	}
	catch (std::exception& e)
	{
		CS_DIE("error occured while reading file " << file << ": " << e.what());
	}
}

void Extractor::scan(const CharType* const str, String::size_type len)
{
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
}

template<uint8_t plen>
void Extractor::scanSentence(const CharType* const str, String::size_type len,
		boost::unordered_map<Phrase<plen>, atimes_t, PhraseHash<plen, _JEBE_BUCKET_BITS> >& phmap)
{
	typedef Phrase<plen> Ph;
	for (String::size_type i = 0, end = len - plen; i < end; ++i)
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

Extractor::Extractor() {
	// TODO Auto-generated constructor stub

}

Extractor::~Extractor() {
	// TODO Auto-generated destructor stub
}

} /* namespace cws */
} /* namespace jebe */
