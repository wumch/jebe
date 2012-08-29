
#pragma once

#include "predef.hpp"
#include <string>
#include <string.h>
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

    inline void endswith(const std::string& _patten, atimes_t _atimes, double freq_)
    {
        patten_end = true;
        patten = _patten;
        atimes_ = _atimes;
        afreq_ = freq_;
    }

    CS_FORCE_INLINE static Node* make_node()
    {
        return new Node;
    }

    CS_FORCE_INLINE const std::string& str() const
    {
    	return patten;
    }

    CS_FORCE_INLINE const Node* const cichildat(byte_t atom) const
    {
    	BOOST_STATIC_ASSERT(('A' | 32) == 'a');
    	if (CS_BUNLIKELY(atom <= 'Z'))
    	{
    		if (CS_BLIKELY('A' <= atom))
    		{
    			return children[atom | 32];
    		}
    	}
    	return children[atom];
    }

    CS_FORCE_INLINE atimes_t atimes() const
    {
    	return atimes_;
    }

    CS_FORCE_INLINE double afreq() const
    {
    	return afreq_;
    }

private:
    Node* children[word_max_len];

    std::string patten;

    bool is_leaf;

    bool patten_end;

    atimes_t atimes_;
    double afreq_;		// frequency of appear.
};

}
}
