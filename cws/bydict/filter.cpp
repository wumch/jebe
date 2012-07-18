
#include "staging.hpp"
#include "filter.hpp"
#include "holders.hpp"
#include <fstream>
#include <iomanip>

namespace jebe {
namespace cws {

std::string Filter::filt(const std::string& str, Atom* const res) const
{
//    memset(res, 0, str.size() << 2);
//    JSONHolder jh(res);
	JoinHolder jh(res);
    find<JoinHolder>(reinterpret_cast<const Atom* const>(str.c_str()), str.size(), jh);
    jh.genRes();
    return std::string(reinterpret_cast<char*>(res));
}

//void Filter::find(const std::string& str, AtomList res,
//    std::size_t max_match) const
//{
//    return find((Atom*)(str.data()), str.size(), res, max_match);
//}
//
//void Filter::find(const char* chs, ContentLen len,
//    AtomList res, ContentLen max_match) const
//{
//    return find((Atom*)(chs), len, res, max_match);
//}
//
//void inline Filter::replace(std::string& str, PosList& poslist) const
//{
//    for (PosList::iterator pos = poslist.begin(); pos != poslist.end(); ++pos)
//    {
//        str.replace(pos->first, pos->second, pos->second, replacement);
//    }
//}

// 优化算法开关。对 匹配几率大 的情况做小幅度优化。
#define NO_REWIND_OPTI 1
#define _JEBE_SCAN_FROM_RIGHT 0

// Filter
template<typename CallbackType>
void Filter::find(const Atom* const atoms, ContentLen len, CallbackType& callback) const
{
	ContentLen matched = 0;
    Node::NodePtr node = tree.root;
#if defined(NO_REWIND_OPTI) && NO_REWIND_OPTI
    bool begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
    for (Cursor i = len - 1, offset = i - 1; i > -1; --i)
    {
    	CS_PREFETCH(node->children, 0, 2);
#else
    for (ContentLen i = 0, offset = 0; i < len ; ++i)
    {
#endif
        if (CS_LIKELY(node = node->children[atoms[i]]))
        {
#if defined(NO_REWIND_OPTI) && NO_REWIND_OPTI
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
                	if (CS_BUNLIKELY(++matched > G::config->max_match))
                	{
                		break;
                	}
					callback(*node);
					node = tree.root;
#if defined(NO_REWIND_OPTI) && NO_REWIND_OPTI
                    begin_from_root = true;
#endif
                }
            }
        }
        else
        {
            node = tree.root;
#if defined(NO_REWIND_OPTI) && NO_REWIND_OPTI
            begin_from_root = true;
#endif
#if _JEBE_SCAN_FROM_RIGHT
            i = offset--;
#else
            i = offset++;
#endif
        }
    }
//    res[wcur - 1] = 0;
}

void Ftree::build(const std::string& fname)
{
    char word[word_max_len + 1];
    memset(word, 0, word_max_len + 1);
    try
    {
        std::ifstream fp(fname.c_str());
#if defined(USE_WCHAR) && USE_WCHAR
        fp.imbue(std::locale(""));
#endif
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

void Ftree::attach_word(const char* word)
{
    const AtomList patten = (AtomList)(word);
    Node::NodePtr node = root;
#if _JEBE_SCAN_FROM_RIGHT
    for(Cursor i = strlen(word) - 1; -1 < i; --i)
#else
    for (WordLen i = 0, end = strlen(word); i < end; ++i)
#endif
    {
        node = node->attach_child(patten[i]);
    }
    node->endswith(word);
}

}
}
