
#include "filter.hpp"
#include <fstream>
#include <iomanip>

namespace jebe {
namespace cws {

// 优化算法开关。对 匹配几率大 的情况做小幅度优化。
#define NO_REWIND_OPTI 0

// Filter
void Filter::find(const AtomList atoms, const ContentLen len,
    AtomList res, const ContentLen max_match) const
{
	CS_SAY(max_match);

    Node::NodePtr node = tree.root;
    ContentLen match_count = 0;
#if defined(NO_REWIND_OPTI) && NO_REWIND_OPTI
    bool begin_from_root = true;
#endif
    Cursor wcur = 0;
    for (Cursor i = len - 1, offset = i - 1; -1 < i; --i)
    {
        if ((node = node->children[atoms[i]]))
        {
#if defined(NO_REWIND_OPTI) && NO_REWIND_OPTI
            if (begin_from_root)
            {
                begin_from_root = false;
                --offset;
            }
#endif
            if (node->patten_end)
            {
                offset = i;
//                res.push_back(Pos(i, node->patten.size()));
                memcpy(res + wcur, node->patten.data(), node->patten.size());
                wcur += node->patten.size();
                res[wcur++] = ' ';

                if (max_match != 0 && max_match < ++match_count)
                {
                    break;
                }
                if (node->is_leaf)
                {
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
            i = offset--;
        }
    }
    res[wcur - 1] = 0;
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
    for(Cursor i = strlen(word) - 1; -1 < i; --i)
    {
        node = node->attach_child(patten[i]);
    }
    node->endswith(word);
}

// Node
inline Node::NodePtr Node::attach_child(const std::size_t _atom)
{
    if (!children[_atom])
    {
        children[_atom] = make_node();
    }
    is_leaf = false;
    return children[_atom];
}

}
}
