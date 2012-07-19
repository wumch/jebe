
#pragma once

#include "predef.hpp"
#include <string>
#include <boost/cstdint.hpp>
#include "holders.hpp"
#include "node.hpp"
#include "config.hpp"

namespace jebe {
namespace cws {

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
};

class Filter
{
private:
    const Ftree& tree;

public:
    explicit Filter(const std::string& fname)
        : tree(Ftree::make_ftree(fname))
    {
    }

    std::string filt(const std::string& str, byte_t* const res) const;

    template<typename CallbackType>
    void find(const byte_t* const atoms, tsize_t len, CallbackType& callback) const;
};

}
}
