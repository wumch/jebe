
#include "filter.hpp"
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include "numcast.hpp"

namespace jebe {
namespace cws {

void Ftree::build(const std::string& fname)
{
	Filter::initCharBytesTable();
	FILE* fp = fopen(fname.c_str(), "rb");
	if (fp)
	{
	    char word[word_max_bytes + 1];
	    double idf = .0;
		while (!feof(fp))
		{
			if (CS_BLIKELY(fscanf(fp, "%s\t%lf\n", word, &idf) == 2))
			{
				CS_SAY("scaned word [" << word << "]");
				attach_word(word, idf);
			}
			else
			{
				CS_DIE("error occured while <Ftree> building. patten-file: " << fname);
			}
		}
	}
	else
	{
		CS_DIE("error occured while opening file: " << fname);
	}
    optimize();
}

void Ftree::attach_word(const char* const word, double idf)
{
    const byte_t* const patten = reinterpret_cast<const byte_t*>(word);
    Node* node = root;
    uint32_t len = strlen(word);
#if _JEBE_SCAN_FROM_RIGHT
    for(int32_t i = len; i > 0; )
    {
    	--i;
#else
    for (wsize_t i = 0; i < len; ++i)
    {
#endif
		node = node->attach_child(patten[i]);
    }
#if _JEBE_SCAN_FROM_RIGHT
//    CS_SAY("[" << std::string(word, len)  << "(" << (int)len << ")"<< "] idf: " << idf);
    node->endswith(std::string(word, len), idf);
#else
    CS_SAY("[" << std::string(word, len) << "] idf: " << idf);
    node->endswith(std::string(word, len), idf);
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
