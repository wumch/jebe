
#pragma once

#include "predef.hpp"
#include <string>
#include "holders.hpp"
#include "node.hpp"
#include "config.hpp"
#include "sendbuff.hpp"

namespace jebe {
namespace cwshttp {

class Ftree
{
public:
    friend class Filter;
    explicit Ftree(const std::string& fname)
        : root(Node::make_node())
    {
        build(fname);
    }

    static Ftree& make_ftree(const std::string& fname)
    {
        static Ftree t(fname);
        return t;
    }

private:
    void build(const std::string& fname);

    void attach_word(const char* word);

    void attach_word(std::string& word);

    Node* root;

    enum { word_max_len = UCHAR_MAX + 1 };
    enum { word_max_bytes = 128 };

    uint64_t total_atimes;
    uint64_t total_words;
};

class Filter
{
protected:
    const Ftree& tree;

public:
    explicit Filter(const std::string& fname)
        : tree(Ftree::make_ftree(fname))
    {
    }

    template<typename CallbackType>
    tsize_t find(const byte_t* const atoms, tsize_t len, CallbackType& callback) const
    {
#if _JEBE_ENABLE_MAXMATCH
    	tsize_t matched = 0;
#endif
    	const Node* node = tree.root;
    	tsize_t offset = 1;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
    	bool begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
    	for (int32_t i = len - 1, offset = i - 1; i > -1; --i)
    	{
    		CS_PREFETCH(node->children, 0, 2);
#else
		for (tsize_t i = 0; i < len ; )
		{
#endif
			if (CS_LIKELY(node = node->cichildat(atoms[i])))
			{
				++i;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
				if (CS_BUNLIKELY(begin_from_root))
				{
					begin_from_root = false;
#	if _JEBE_SCAN_FROM_RIGHT
					--offset;
#	else
					++offset;
#	endif
				}
#endif
				if (CS_BUNLIKELY(node->patten_end))
				{
//					offset = i;
					callback(*node);
					if (CS_BLIKELY(node->is_leaf))
					{
#if _JEBE_ENABLE_MAXMATCH
						if (CS_BUNLIKELY(Config::getInstance()->max_match))
						{
							if (CS_BUNLIKELY(++matched > Config::getInstance()->max_match))
							{
								break;
							}
						}
#endif
						node = tree.root;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
						begin_from_root = true;
#endif
					}
				}
			}
			else
			{
				node = tree.root;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
				begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
				i = offset--;
#else
				i = ++offset;
#endif
			}
		}
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
		return std::min(offset, _JEBE_WORD_MAX_LEN);
#else
		return std::max(offset, len > _JEBE_WORD_MAX_LEN ? len - _JEBE_WORD_MAX_LEN : len);
#endif
	}

};

}
}
