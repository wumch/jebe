
#include "filter.hpp"
#include <fstream>
#include <iomanip>
#include "numcast.hpp"

namespace jebe {
namespace cws {

void Ftree::build(const std::string& fname)
{
	Filter::initCharBytesTable();
	words.reserve(1 << 20);
    char word[word_max_bytes];
    memset(word, 0, word_max_bytes);
    try
    {
        std::ifstream fp(fname.c_str());
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
    words_atime.resize(words.size());
}

void Ftree::attach_word(const char* const word)
{
    const byte_t* const patten = reinterpret_cast<const byte_t*>(word);
    Node* node = root;
    wsize_t i = 0;
    uint remain_bytes = 0;
#if _JEBE_SCAN_FROM_RIGHT
    for(int32_t i = strlen(word) - 1; -1 < i; --i)
#else
    for (wsize_t end = strlen(word); (i < end) && notEdge(patten[i]); ++i)
#endif
    {
		if (remain_bytes == 0)
		{
			assert(Filter::charBytes(patten[i]));
			remain_bytes = Filter::charBytes(patten[i]);
			assert(remain_bytes);
		}
		node = node->attach_child(patten[i], --remain_bytes);
    }
    static uint32_t nodeid = 0;
    node->endswith(nodeid++);
    words.push_back(std::string(word, i));
}

uint8_t Filter::char_bytes_table[char_bytes_talbe_size];

void Filter::dumpTree() const
{
	std::ofstream ofile(Config::getInstance()->outputfile.string().c_str(), std::ios_base::trunc);

	uint64_t total_atimes = 0;
	for (Ftree::WordsAtime::const_iterator it = tree.words_atime.begin(); it != tree.words_atime.end(); ++it)
	{
		total_atimes += *it;
	}
	ofile << tree.words.size() << " " << total_atimes << std::endl;

	for (size_t i = 0; i < tree.words.size(); ++i)
	{
		ofile << tree.words[i] << '\t' << tree.words_atime[i] << "\n";
	}

	ofile.close();
}

}
}
