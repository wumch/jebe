
#pragma once

#include "staging.hpp"
#include "predef.hpp"
#include <string>
#include "array.hpp"

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

    inline NodePtr attach_child(const std::size_t _atom)
    {
    	if (!children[_atom])
		{
			children[_atom] = make_node();
		}
		is_leaf = false;
		return children[_atom];
    }

    inline void endswith(const std::string& _patten)
    {
        patten_end = true;
        patten = _patten;
    }

    static NodePtr CS_FORCE_INLINE make_node()
    {
        return new Node;
    }

    const std::string& str() const
    {
    	return patten;
    }

private:
    static const std::size_t word_max_len = atom_ubound;
    NodePtr children[word_max_len];

    std::string patten;

    bool is_leaf;

    bool patten_end;
};

}
}
