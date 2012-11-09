
#include "filter.hpp"
#include <fstream>
#include <iomanip>
#include "numcast.hpp"

namespace jebe {
namespace idf {

void Ftree::attach_word(const Word& word, wordid_t pattenid_)
{
	attach_word(word.data(), word.size(), pattenid_);
}

void Ftree::attach_word(const char* const word, size_t wordlen, wordid_t pattenid_)
{
    const byte_t* const patten = reinterpret_cast<const byte_t*>(word);
    Node* node = root;
#if _JEBE_SCAN_FROM_RIGHT
    for(int32_t i = wordlen - 1; 0 <= i; --i)
    {
		node = node->attach_child(patten[i]);
    }
#else
    for (int i = 0, end = wordlen; i < end; ++i)
    {
    	node = node->attach_child(patten[i]);
    }
#endif
    node->endswith(pattenid_);
}

// suppose being-matched-content utf-8 bytes.
void Ftree::optimize()
{
	cache_children(root);
	for (uint i = 0; i < root->childrenum; ++i)
	{
		cache_children(root->children[i]);
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
