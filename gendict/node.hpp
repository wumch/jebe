
#pragma once

#include "ppredef.hpp"
#include <cassert>
#include <string>
#include <string.h>
#include <boost/array.hpp>
#include <boost/pool/pool_alloc.hpp>

namespace jebe {
namespace cws {

class Node;
static Node* make_node(byte_t _atom, byte_t remain_bytes);

class Node
{
public:
    friend class Filter;
    friend class Ftree;

    enum { word_max_len = UCHAR_MAX + 1 };

    explicit Node(byte_t _atom, byte_t remain_bytes)
    	: children(NULL), pattenid(0), childrenum(0), atom(_atom), is_leaf(true), patten_end(false),
    	  is_char_begins_(testCharBegins(atom)), remain_bytes_(remain_bytes), step_forward(remain_bytes_ + 1)
    {
    }

    Node* attach_child(byte_t _atom, byte_t remain_bytes)
    {
		is_leaf = false;
    	return insert_child(_atom, remain_bytes);
    }

    Node* insert_child(byte_t _atom, byte_t remain_bytes)
    {
		if (CS_BUNLIKELY(childrenum == 0))
		{
			children = reinterpret_cast<Node**>(malloc(sizeof(Node*)));
			childrenum = 1;
			return children[0] = make_node(_atom, remain_bytes);
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

		return insert_child_at(_atom, remain_bytes, right + (children[right]->atom < _atom));
    }

    Node* insert_child_at(byte_t child_atom, byte_t remain_bytes, byte_t index)
    {
    	children = reinterpret_cast<Node**>(realloc(children, (childrenum + 1) * sizeof(Node*)));
    	assert(children != NULL);
    	for (int i = childrenum; i > index; --i)
    	{
    		move_child(i - 1, i);
    	}
    	children[index] = make_node(child_atom, remain_bytes);
    	++childrenum;
    	return children[index];
    }

    void move_child(byte_t from, byte_t to)
    {
    	CS_PREFETCH(children[from], 0, 1);
    	children[to] = children[from];
    }

    void endswith(uint32_t _pattenid)
    {
        patten_end = true;
        pattenid = _pattenid;
    }

    Node* find_child(byte_t _atom) const
    {
    	if (CS_BUNLIKELY(childrenum == 0))
    	{
    		return NULL;
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
//#define _JEBE_OUT_CONFIG_PROPERTY(property)		<< CS_OC_GREEN(#property) << ":\t" << CS_OC_RED(property) << std::endl
//    	CS_SAY(""
//			_JEBE_OUT_CONFIG_PROPERTY(childrenum)
//			_JEBE_OUT_CONFIG_PROPERTY(_atom)
//			_JEBE_OUT_CONFIG_PROPERTY(left)
//			_JEBE_OUT_CONFIG_PROPERTY(right)
//			_JEBE_OUT_CONFIG_PROPERTY(cur)
//		);

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

    CS_FORCE_INLINE Node* cichildat(byte_t child_atom) const
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

    CS_FORCE_INLINE uint8_t remainBytes() const
    {
    	return remain_bytes_;
    }

    CS_FORCE_INLINE uint8_t isCharBegins() const
    {
    	return is_char_begins_;
    }

    CS_FORCE_INLINE static bool testCharBegins(const byte_t _atom)
    {
    	return _atom > 127 || _atom > 192;
    }

protected:	// sort declartion of data-fields for memory saving...
    Node** children;
    uint32_t pattenid;

    byte_t childrenum;

    byte_t atom;

    uint8_t is_leaf:1;
    uint8_t patten_end:1;
    uint8_t is_char_begins_:1;
    uint8_t remain_bytes_:3;
    uint8_t step_forward:2;
};

template<int id> class PoolTag {};
typedef boost::singleton_pool<PoolTag<1>, sizeof(Node), boost::default_user_allocator_malloc_free, boost::details::pool::null_mutex> NodePool;
typedef boost::singleton_pool<PoolTag<2>, sizeof(Node*), boost::default_user_allocator_malloc_free, boost::details::pool::null_mutex> NodePtrPool;

static Node* make_node(byte_t _atom, byte_t remain_bytes)
{
	return new (NodePool::malloc()) Node(_atom, remain_bytes);
}

}
}
