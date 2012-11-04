
#include "filter.hpp"
#include <fstream>
#include <iomanip>
#include "numcast.hpp"

namespace jebe {
namespace ics {

void Ftree::build(const std::string& fname)
{
	Filter::initCharBytesTable();
    char word[word_max_bytes];
    memset(word, 0, word_max_bytes);
    try
    {
        std::ifstream fp(fname.c_str());
        if (fp.getline(word, word_max_bytes))
        {
        	size_t consumed_bytes = 0;
        	total_words = staging::NumCast::strtoul_comma(word, &consumed_bytes);
        	CS_SAY("consumed_bytes: " << consumed_bytes);
        	total_atimes = staging::NumCast::strtoul_comma(word + consumed_bytes + 1);
        	CS_SAY("total_words: " << total_words);
        	CS_SAY("total_atimes: " << total_atimes);
        }

        if (fp)
        {
            while (fp.getline(word, word_max_bytes))
            {
                attach_word(word);
            }
        }
    }
    catch (const std::exception& e)
    {
        CS_SAY("error occured while <Ftree> building: " << e.what());
        exit(1);
    }
}

void Ftree::attach_word(const char* const word)
{
    const byte_t* const patten = reinterpret_cast<const byte_t*>(word);
    Node* node = root;
    atimes_t atimes = 0;
    wsize_t i = 0;
    uint remain_bytes = 0;
#if _JEBE_SCAN_FROM_RIGHT
    for(int32_t i = strlen(word) - 1; -1 < i; --i)
#else
    for (wsize_t end = strlen(word); i < end; ++i)
#endif
    {
    	if (CS_BUNLIKELY(patten[i] == '\t'))
    	{
    		atimes = staging::NumCast::strtoul_comma(patten + i + 1);
    		break;
    	}
    	else
    	{
    		if (remain_bytes == 0)
    		{
    			assert(Filter::charBytes(patten[i]));
				remain_bytes = Filter::charBytes(patten[i]);
				assert(remain_bytes);
    		}
    		node = node->attach_child(patten[i], --remain_bytes);
    	}
    }
    CS_SAY("[" << std::string(word, i) << "] atimes: " << atimes << ", total_atimes: " << total_atimes << ", freq: " << (static_cast<double>(total_atimes) / atimes));
    node->endswith(std::string(word, i), atimes, static_cast<double>(total_atimes) / atimes);
}

uint8_t Filter::char_bytes_table[char_bytes_talbe_size];

}
}
