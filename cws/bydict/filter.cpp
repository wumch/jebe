
#include "filter.hpp"
#include "holders.hpp"
#include <fstream>
#include <iomanip>
#include <boost/tuple/detail/tuple_basic.hpp>

namespace jebe {
namespace cws {

tsize_t Filter::filt(const byte_t* const str, tsize_t size, byte_t* const res) const
{
	JSONHolder jh(res);
    find<>(str, size, jh);
    jh.genRes();
    return jh.size();
//    return reinterpret_cast<char*>(res);
}

// Filter
template<typename CallbackType>
void Filter::find(const byte_t* const atoms, tsize_t len, CallbackType& callback) const
{
#if _JEBE_ENABLE_MAXMATCH
	tsize_t matched = 0;
#endif
    const Node* node = tree.root;
#if defined(_JEBE_NO_REWIND_OPTI) && _JEBE_NO_REWIND_OPTI
    bool begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
    for (int32_t i = len - 1, offset = i - 1; i > -1; --i)
    {
    	CS_PREFETCH(node->children, 0, 2);
#else
    for (tsize_t i = 0, offset = 0; i < len ; ++i)
    {
#endif
        if (CS_LIKELY(node = node->cichildat(atoms[i])))
        {
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
                offset = i;
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
					callback(*node);
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
            i = offset++;
#endif
        }
    }
}

void Ftree::build(const std::string& fname)
{
    char word[word_max_len + 1];
    memset(word, 0, word_max_len + 1);
    try
    {
        std::ifstream fp(fname.c_str());
        if (fp)
        {
            while (fp.getline(word, word_max_len))
            {
                attach_word(word);
            }
        }
    }
    catch (std::exception& e)
    {
        CS_SAY("error occured while <Ftree> building: " << e.what());
        exit(1);
    }
}

void Ftree::attach_word(const char* const word)
{
    const byte_t* const patten = reinterpret_cast<const byte_t*>(word);
    Node* node = root;
#if _JEBE_SCAN_FROM_RIGHT
    for(int32_t i = strlen(word) - 1; -1 < i; --i)
#else
    for (wsize_t i = 0, end = strlen(word); i < end; ++i)
#endif
    {
        node = node->attach_child(patten[i]);
    }
    node->endswith(word);
}

}
}
