
#pragma once

#include "predef.hpp"
#include <cassert>
#include <string>
#include <string.h>
#include <boost/array.hpp>
#include <boost/pool/pool_alloc.hpp>

namespace jebe {
namespace cws {

class Node;
static Node* make_node(byte_t _atom);

template<int id> class PoolTag {};
typedef boost::singleton_pool<PoolTag<2>, sizeof(Node*), boost::default_user_allocator_malloc_free, boost::details::pool::null_mutex, 20 << 20> NodePtrPool;

class Node
{
public:
    friend class Filter;
    friend class Ftree;

    enum { word_max_len = UCHAR_MAX + 1 };

    explicit Node(byte_t _atom)
    	: children(NULL), childrenum(0), atom(_atom), is_leaf(true), patten_end(false)
    {
    }

    Node* attach_child(byte_t _atom)
    {
		is_leaf = false;
    	return insert_child(_atom);
    }

    Node* insert_child(byte_t _atom)
    {
    	if (childrenum == 0)
		{
			return insert_child_at(_atom, 0);
		}
		int left = 0, right = childrenum - 1, cur;
		if (children[left]->atom == _atom)
		{
			return children[left];
		}
		else if(children[right]->atom == _atom)
		{
			return children[right];
		}
		while (left <= right)
		{
			cur = (left + right) >> 1;
			if (children[cur]->atom == _atom)
			{
				return children[cur];
			}
			if (children[cur]->atom < _atom)
			{
				left = cur + 1;
			}
			else
			{
				right = cur - 1;
			}
		}
		return insert_child_at(_atom, left);
    }

    Node* insert_child_at(byte_t child_atom, byte_t index)
    {
    	const Node* const* old_children = children;
    	children = reinterpret_cast<Node**>(NodePtrPool::ordered_malloc(childrenum + 1));
    	assert(children != NULL);
		memcpy(children, old_children, index * sizeof(Node*));
    	children[index] = make_node(child_atom);
    	memcpy(children + index + 1, old_children + index, (childrenum - index) * sizeof(Node*));
    	++childrenum;
    	return children[index];
    }

    void move_child(byte_t from, byte_t to)
    {
    	children[to] = children[from];
    }

    void endswith(const std::string& _patten, double freq_)
    {
        patten_end = true;
        patten = _patten;
        afreq_ = freq_;
    }

    Node* find_child(byte_t _atom) const
    {
    	int left = 0, right = childrenum - 1, cur;
    	while (left <= right)
    	{
    		cur = (left + right) >> 1;
    		if (children[cur]->atom == _atom)
    		{
    			return children[cur];
    		}
    		if (children[cur]->atom < _atom)
    		{
    			left = cur + 1;
    		}
    		else
    		{
    			right = cur - 1;
    		}
    	}
		return NULL;
    }

    CS_FORCE_INLINE const std::string& str() const
    {
    	return patten;
    }

    CS_FORCE_INLINE const Node* cichildat(byte_t child_atom) const
    {
    	BOOST_STATIC_ASSERT(('A' | 32) == 'a');
    	if (CS_BUNLIKELY(child_atom <= 'Z'))
    	{
    		if (CS_BLIKELY('A' <= child_atom))
    		{
    			return find_child(child_atom | 32);
    		}
    	}
    	return find_child(child_atom);
    }

    CS_FORCE_INLINE double afreq() const
    {
    	return afreq_;
    }

    CS_FORCE_INLINE static bool testCharBegins(const byte_t _atom)
    {
    	return _atom > 127 || _atom > 192;
    }

protected:	// sort declartion of data-fields for memory saving...
    std::string patten;
    Node** children;

    double afreq_;		// frequency of appear.

    byte_t childrenum;

    byte_t atom;

    bool is_leaf;
    bool patten_end;
};

typedef boost::singleton_pool<PoolTag<1>, sizeof(Node), boost::default_user_allocator_malloc_free, boost::details::pool::null_mutex, 20 << 20> NodePool;

static Node* make_node(byte_t _atom)
{
	return new (NodePool::malloc()) Node(_atom);
}

}
}
