
#pragma once

#include "predef.hpp"
#include <cassert>
#include <string>
#include <string.h>
#include <boost/array.hpp>
#include <boost/pool/pool_alloc.hpp>
#include "aside.hpp"

namespace jebe {
namespace idf {

class Node;
static Node* make_node(byte_t _atom);

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
		if (CS_BUNLIKELY(childrenum == 0))
		{
			children = reinterpret_cast<Node**>(malloc(sizeof(Node*)));
			childrenum = 1;
			return children[0] = make_node(_atom);
		}

		// hopefully avoid from loop.
		if (children[0]->atom == _atom)
		{
			return children[0];
		}

		uint left = 0, right = childrenum - 1;
		uint cur = (left + right) >> 1;

		// avoid from loop if only two atoms.
		while (left < cur && cur < right)
		{
			if (children[cur]->atom == _atom)
			{
				return children[cur];
			}
			else if (children[cur]->atom < _atom)
			{
				left = cur;
			}
			else
			{
				right = cur;
			}
			cur = (left + right) >> 1;
		}

		if (children[left]->atom == _atom)
		{
			return children[left];
		}
		else if (children[right]->atom == _atom)
		{
			return children[right];
		}

		return insert_child_at(_atom, right + (children[right]->atom < _atom));
    }

    Node* insert_child_at(byte_t child_atom, byte_t index)
    {
    	children = reinterpret_cast<Node**>(realloc(children, (childrenum + 1) * sizeof(Node*)));
    	assert(children != NULL);
    	for (uint i = childrenum; i > index; --i)
    	{
    		move_child(i - 1, i);
    	}
    	children[index] = make_node(child_atom);
    	++childrenum;
    	return children[index];
    }

    void move_child(byte_t from, byte_t to)
    {
    	children[to] = children[from];
    }

    void endswith(wordid_t pattenid_)
    {
    	pattenid = pattenid_;
        patten_end = true;
    }

    Node* find_child(byte_t _atom) const
    {
    	if (CS_BUNLIKELY(childrenum == 0))
    	{
    		return NULL;
    	}

    	// hopefully avoid from loop.
    	if (children[0]->atom == _atom)
    	{
    		return children[0];
    	}

    	uint left = 0, right = childrenum - 1;
    	uint cur = (left + right) >> 1;
    	while (left < cur && cur < right)
    	{
    		if (children[cur]->atom == _atom)
    		{
    			return children[cur];
    		}
    		if (children[cur]->atom < _atom)
    		{
    			left = cur;
    		}
    		else
    		{
    			right = cur;
    		}
    		cur = (left + right) >> 1;
    	}

		if (children[right]->atom == _atom)
		{
			return children[right];
		}
		else if (children[left]->atom == _atom)
		{
			return children[left];
		}
		return NULL;
    }

    CS_FORCE_INLINE const std::string& str() const
    {
    	return Aside::wordList[pattenid];
    }

    CS_FORCE_INLINE const Node* const cichildat(byte_t child_atom) const
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

public:
    uint32_t pattenid;

protected:	// sort declartion of data-fields for memory saving...
    Node** children;

    byte_t childrenum;

    byte_t atom;

    uint8_t is_leaf:1;
    uint8_t patten_end:1;
};

template<int id> class PoolTag {};
typedef boost::singleton_pool<PoolTag<1>, sizeof(Node), boost::default_user_allocator_malloc_free, boost::details::pool::null_mutex> NodePool;
typedef boost::singleton_pool<PoolTag<2>, sizeof(Node*), boost::default_user_allocator_malloc_free, boost::details::pool::null_mutex> NodePtrPool;

static Node* make_node(byte_t _atom)
{
	return new (NodePool::malloc()) Node(_atom);
}

}
}
