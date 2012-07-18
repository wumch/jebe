
#pragma once

#define USE_WCHAR 0

#include "staging.hpp"
#include "predef.hpp"
#include <string>
#include <boost/cstdint.hpp>
#include "holders.hpp"
#include "array.hpp"
#include "node.hpp"
#include "shareinfo.hpp"

namespace jebe {
namespace cws {

//bool CS_FORCE_INLINE operator==(const Node* const n1, const Node* const n2);

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

    explicit Filter(const std::string& fname)
        : tree(Ftree::make_ftree(fname))
//    ,res(new Atom[G::config->body_max_size << 1])
    {
//		res = new Atom[G::config->body_max_size << 1];
    }

    std::string filt(const std::string& str, Atom* const res) const;

    void inline find(const std::string& str, AtomList res,
        const std::size_t max_match) const;

    void inline find(const char* chs, ContentLen len,
        AtomList res, ContentLen max_match) const;

    template<typename CallbackType>
    void find(const Atom* const atoms, ContentLen len, CallbackType& callback) const;

    void inline replace(std::string& str, PosList& poslist) const;

    void inline replace(Content& content, const PosList& poslist) const;

private:
    void CS_FORCE_INLINE record(PosList& poslist, const Pos& pos) const;

    const Ftree& tree;

//    mutable Atom* res;

//    mutable JoinHolder joinHolder;
};

}
}
