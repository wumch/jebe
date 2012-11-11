
#pragma once

#include "predef.hpp"
#include <string>
#include "node.hpp"
#include "config.hpp"

namespace jebe {
namespace idf {

class Ftree
{
public:
    friend class Filter;
    explicit Ftree()
    	: root(make_node(0))
    {}

    void attach_word(const Word& word, wordid_t pattenid_);

private:
    enum { word_max_len = UCHAR_MAX + 1 };
    enum { word_max_bytes = 128 };

    void attach_word(const char* const word, size_t wordlen, wordid_t pattenid_);

    Node* const root;

private:
    void optimize();

    void cache_children(const Node* node);
};

class Filter
{
protected:
	enum { char_bytes_talbe_size = (1 << CHAR_BIT) };
    static uint8_t char_bytes_table[char_bytes_talbe_size];

    const Ftree* tree;

public:
    explicit Filter()
    	: tree(NULL)
    {}

    void attachTree(const Ftree* ftree)
    {
    	tree = ftree;
    }

    CS_FORCE_INLINE static uint charBytes(const byte_t _atom)
    {
    	return char_bytes_table[_atom];
    }

    static void initCharBytesTable()
    {
    	for (uint i = 0; i < char_bytes_talbe_size; ++i)
    	{
    		char_bytes_table[i] = genCharBytes(i);
    	}
    }

    static uint genCharBytes(const byte_t _atom)
    {
    	if (_atom & (1 << 7))
    	{
			if (CS_BLIKELY(_atom & (1 << 6)))
			{
				if (CS_BLIKELY(_atom & (1 << 5)))
				{
					if (CS_BUNLIKELY(_atom & (1 << 4)))
					{
						if (CS_BUNLIKELY(_atom & (1 << 3)))
						{
							if (CS_BUNLIKELY(_atom & (1 << 2)))
							{
								if (CS_BUNLIKELY(_atom & (1 << 1)))
								{
									return 1;	// error occured.
								}
								return 6;
							}
							return 5;
						}
						return 4;
					}
					return 3;
				}
				return 2;
			}
			return 1;
    	}
    	return 1;
    }

    template<typename CallbackType>
    void find(const byte_t* const atoms, tsize_t len, CallbackType& callback) const
    {
#if _JEBE_ENABLE_MAXMATCH
    	tsize_t matched = 0;
#endif
    	const Node* node = tree->root;
    	int32_t offset = len - 1;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
    	bool begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
    	for (int32_t i = len - 1; i > -1; )
    	{
#else
		for (tsize_t i = 0; i < len ; )
		{
#endif
			if (CS_LIKELY(node = node->cichildat(atoms[i])))
			{
#if _JEBE_SCAN_FROM_RIGHT
				--i;
#else
				++i;
#endif
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
					callback(node);
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
						node = tree->root;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
						begin_from_root = true;
#endif
					}
				}
			}
			else
			{
				node = tree->root;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
				begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
				i = --offset;
#else
#	if defined(_JEBE_STEP_FWD_OPTI) && _JEBE_STEP_FWD_OPTI
				i = (offset += charBytes(atoms[offset]));
#	else
				i = ++offset;
#	endif
#endif
			}
		}
	}

};

}
}