
#pragma once

#define USE_WCHAR 0

#include <vector>
#include <string>
#include <boost/cstdint.hpp>
#include "predef.hpp"
#include "../../staging/array.hpp"

#if defined(USE_WCHAR) && USE_WCHAR
#include <wchar.h>
typedef wchar_t Atom;
typedef std::wstring Word;
#else
typedef unsigned char Atom;
typedef std::basic_string<Atom> Word;
#endif

typedef Atom* AtomList;
typedef Word Content;
typedef uint32_t ContentLen;
typedef uint8_t WordLen;
typedef int32_t Cursor;

namespace jebe {
namespace cws {

static const std::size_t atom_ubound = 1 << (sizeof(Atom) * 8);

class Node
{
public:
    friend class Filter;
    friend class Ftree;

    typedef Node* NodePtr;
    typedef staging::Array<NodePtr, atom_ubound> Children;

    explicit Node()
        : is_leaf(true), patten_end(false)
    {
        memset(children, 0, sizeof(children));
    }

    inline NodePtr attach_child(const std::size_t _atom);

    inline void endswith(const std::string& _patten)
    {
        patten_end = true;
        patten = _patten;
    }

    static NodePtr CS_FORCE_INLINE make_node()
    {
        return new Node();
    }

private:
    bool is_leaf;

    bool patten_end;

    std::string patten;

    static const std::size_t word_max_len = atom_ubound;

    NodePtr children[word_max_len];
};

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

    Node::NodePtr root;

    static const std::size_t word_max_len = 1 << 8;
};

class Filter
{
public:
    typedef std::pair<ContentLen, WordLen> Pos;
    typedef std::vector<Pos> PosList;

    explicit Filter(const std::string& fname, const char _replacement = '*')
        : replacement(_replacement), tree(Ftree::make_ftree(fname))
    {}

    std::string inline filt(const std::string& str, const ContentLen max_match = 100) const;

    void inline find(const std::string& str, AtomList res,
        const std::size_t max_match) const;

    void inline find(const char* chs, ContentLen len,
        AtomList res, ContentLen max_match) const;

    void find(const AtomList atoms, ContentLen len,
        AtomList res, ContentLen max_match) const;

    void inline replace(std::string& str, PosList& poslist) const;

    void inline replace(Content& content, const PosList& poslist) const;

private:
    void CS_FORCE_INLINE record(PosList& poslist, const Pos& pos) const;

    const char replacement;

    const Ftree& tree;
};

std::string Filter::filt(const std::string& str, ContentLen max_match) const
{
    AtomList res = new Atom[str.size() << 1];
    memset(res, 0, str.size() << 1);
    find(str, res, max_match);
//    CS_SAY(res);
//	std::cout << str << std::endl
//		<< res << std::endl;
    return std::string(reinterpret_cast<char*>(res));
}

void Filter::find(const std::string& str, AtomList res,
    std::size_t max_match) const
{
    return find((Atom*)(str.data()), str.size(), res, max_match);
}

void Filter::find(const char* chs, ContentLen len,
    AtomList res, ContentLen max_match) const
{
    return find((Atom*)(chs), len, res, max_match);
}

void inline Filter::replace(std::string& str, PosList& poslist) const
{
    for (PosList::iterator pos = poslist.begin(); pos != poslist.end(); ++pos)
    {
        str.replace(pos->first, pos->second, pos->second, replacement);
    }
}

}
}
