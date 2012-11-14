
#include "filter.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "numcast.hpp"

namespace jebe {
namespace cws {

void Ftree::build(const std::string& fname)
{
	Filter::initCharBytesTable();
    char word[word_max_bytes];
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
    optimize();
}

void Ftree::attach_word(const char* const word)
{
    const byte_t* const patten = reinterpret_cast<const byte_t*>(word);
    Node* node = root;
    double idf = 0;
#if _JEBE_SCAN_FROM_RIGHT
    wsize_t len = 0;
    for(int32_t i = strlen(word); i > 0; )
    {
    	--i;
#else
	wsize_t i = 0;
    for (wsize_t end = strlen(word); i < end; ++i)
    {
#endif
    	if (patten[i] == '\t')
    	{
    		idf = boost::lexical_cast<double>(patten + i + 1);
#if _JEBE_SCAN_FROM_RIGHT
    		len = i;
#else
    		break;
#endif
    	}
#if _JEBE_SCAN_FROM_RIGHT
    	else if (len)
#else
    	else
#endif
    	{
    		node = node->attach_child(patten[i]);
    	}
    }
#if _JEBE_SCAN_FROM_RIGHT
//    CS_SAY("[" << std::string(word, len)  << "(" << (int)len << ")"<< "] idf: " << idf);
    node->endswith(std::string(word, len), idf);
#else
    CS_SAY("[" << std::string(word, i) << "] idf: " << idf);
    node->endswith(std::string(word, i), idf);
#endif
}

// suppose being-matched-content utf-8 bytes.
void Ftree::optimize()
{
	const Node* parent_0 = root;
	cache_children(parent_0);
	for (uint i_0 = 0; i_0 < parent_0->childrenum; ++i_0)
	{
		const Node* parent_1 = parent_0->children[i_0];
		cache_children(parent_1);
		for (uint i_1 = 0; i_1 < parent_1->childrenum; ++i_1)
		{
			const Node* parent_2 = parent_1->children[i_1];
			cache_children(parent_2);
			for (uint i_2 = 0; i_2 < parent_2->childrenum; ++i_2)
			{
				const Node* parent_3 = parent_2->children[i_2];
				cache_children(parent_3);
				for (uint i_3 = 0; i_3 < parent_3->childrenum; ++i_3)
				{
					const Node* parent_4 = parent_3->children[i_3];
					cache_children(parent_4);
				}
			}
		}
	}
}

void Ftree::cache_children(const Node* node)
{
	for (uint i = 0; i < node->childrenum; ++i)
	{
		CS_PREFETCH(node->children[i], 0, 3);
	}
}

uint8_t Filter::char_bytes_table[char_bytes_talbe_size];

}
}
