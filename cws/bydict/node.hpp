
#pragma once

#include "predef.hpp"
#include <string>
#include <boost/array.hpp>

namespace jebe {
namespace cws {

class Node
{
public:
    friend class Filter;
    friend class Ftree;

    enum { word_max_len = UCHAR_MAX + 1 };
    typedef boost::array<Node*, word_max_len> Children;

    explicit Node()
        : is_leaf(true), patten_end(false)
    {
        memset(children, 0, sizeof(children));
    }

    inline Node* attach_child(byte_t _atom)
    {
    	if (!children[_atom])
		{
			children[_atom] = make_node();
		}
		is_leaf = false;
		return children[_atom];
    }

    inline void endswith(const pstr& _patten)
    {
        patten_end = true;
        patten = _patten;
    }

    CS_FORCE_INLINE static Node* make_node()
    {
        return new Node;
    }

    CS_FORCE_INLINE const pstr& str() const
    {
    	return patten;
    }

    CS_FORCE_INLINE const Node* const cichildat(byte_t atom) const
    {
    	return CS_BUNLIKELY('A' <= atom && atom <= 'Z') ? children[atom + 32] : children[atom];
    }

private:
    Node* children[word_max_len];

    pstr patten;

    bool is_leaf;

    bool patten_end;
};

}
}