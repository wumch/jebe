
#pragma once

#include "ppredef.hpp"
#include <string>
#include "node.hpp"
#include "config.hpp"

namespace jebe {
namespace cws {

class Ftree
{
    friend class Filter;
public:
    explicit Ftree(const std::string& fname)
        : root(make_node(0, 0))
    {
        build(fname);
    }

    static Ftree& make_ftree(const std::string& fname)
    {
        static Ftree t(fname);
        return t;
    }
protected:
    CS_FORCE_INLINE static bool notEdge(const byte_t c)
    {
    	return c > 44;		// chr(44) is '-'
//    	return c != '\n' && c != ' ' && c != '\t';
    }

public:
    typedef std::vector<std::string> Words;
    typedef std::vector<atimes_t> WordsAtime;
    Words words;
    WordsAtime words_atime;

private:
    enum { word_max_len = UCHAR_MAX + 1 };
    enum { word_max_bytes = 128 };

    void build(const std::string& fname);

    void attach_word(const char* word);

    void attach_word(std::string& word);

    Node* root;
};

class Filter
{
protected:
	enum { char_bytes_talbe_size = (1 << CHAR_BIT) };
    static uint8_t char_bytes_table[char_bytes_talbe_size];

    Ftree& tree;

public:
    explicit Filter(const std::string& fname)
        : tree(Ftree::make_ftree(fname))
    {
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

    void dumpTree() const;

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

    void handle_match(Node* node)
    {
    	++tree.words_atime[node->pattenid];
    }

    tsize_t find(const byte_t* const atoms, tsize_t len)
    {
#if _JEBE_ENABLE_MAXMATCH
    	tsize_t matched = 0;
#endif
    	Node* node = tree.root;
    	int32_t offset = 0;
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
			if ((node = node->cichildat(atoms[i])))
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
				if (node->patten_end)
				{
					handle_match(node);
					if (node->is_leaf)
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
				// jump more iff `atoms[i]` is impossible to appear in pattens.
				node = tree.root;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
				begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
				i = offset--;
#else
#	if defined(_JEBE_STEP_FWD_OPTI) && _JEBE_STEP_FWD_OPTI
				i = (offset += charBytes(atoms[offset]));
#	else
				i = ++offset;
#	endif
#endif
			}
		}
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
		return std::min(offset, _JEBE_WORD_MAX_LEN);
#else
		return std::max(offset, static_cast<int32_t>(len > _JEBE_WORD_MAX_LEN ? len - _JEBE_WORD_MAX_LEN : len));
#endif
	}

};

}
}
