
#pragma once

#include "staging.hpp"
#include <boost/unordered_map.hpp>
#include "predef.hpp"
#include "node.hpp"

namespace jebe {
namespace cws {

class Node;

// join pattens with space.
class JoinHolder
{
	AtomList res;
	ContentLen cur;
public:
	JoinHolder(AtomList res_): res(res_), cur(0) {}

	void operator()(const Node& node)
	{
		memcpy(res + cur, node.str().data(), node.str().size());
		cur += node.str().size();
		res[cur++] = ' ';
	}

	void genRes() const
	{
		if (cur > 0)
		{
			res[cur - 1] = 0;
		}
	}
};
//
//extern bool CS_FORCE_INLINE operator==(const Node* const n1, const Node* const n2);
//static bool operator==(const Node* const n1, const Node* const n2)
//{
//	return n1 == n2;
//}

class NodeHash
{
	uint32_t mask;
public:
	uint32_t operator()(const Node* const node) const
	{
		return (size_t)(node) & mask;
	}
};

// hold words and format to JSON.
class JSONHolder
{
public:
	typedef boost::unordered_map<const Node* const, atimes_t, NodeHash> Words;
	Words words;
	AtomList res;
	mutable ContentLen cur;
	static const int16_t mid_joiner =
#if CS_IS_LITTLE_ENDIAN
			(':' << CHAR_BIT) + '"';
#else
			('"' << CHAR_BIT) + ':';
#endif
			static const int16_t end_joiner =
#if CS_IS_LITTLE_ENDIAN
					(',' << CHAR_BIT) + '"';
#else
					('"' << CHAR_BIT) + ',';
#endif
	static const int8_t tail =
#if CS_IS_LITTLE_ENDIAN
			'}';
#else
			'}' << CHAR_BIT;
#endif

	std::pair<const Node*, atimes_t> word_atime;		// assit to make no stack alloc.


	mutable char convbuf[11];

public:
	JSONHolder(AtomList res_):
		words(128), res(res_), cur(1), word_atime(NULL, 1)
	{
		res[0] = '{';
	}

	void operator()(const Node& node)
	{
		Words::iterator it = words.find(&node);
		if (CS_BUNLIKELY(it == words.end()))
		{
			word_atime.first = &node;
			words.insert(word_atime);
		}
		else
		{
			it->second++;
		}
	}

//	const Atom*
	void genRes() const
	{
		for (Words::const_iterator it = words.begin(); it != words.end(); ++it)
		{
			if (CS_BLIKELY(it != words.begin()))
			{
				res[cur++] = ',';
			}
			res[cur++] = '"';

			memcpy(res + cur, it->first->str().data(), it->first->str().size());
			cur += it->first->str().size();

			*reinterpret_cast<int16_t*>(res + cur) = mid_joiner;
			cur += sizeof(mid_joiner);

			appendAtimes(it->second);
		}
		*reinterpret_cast<int16_t*>(res + cur) = tail;
		cur += 1;
	}

	void appendAtimes(atimes_t atimes) const
	{
		if (CS_BLIKELY(atimes < 10))
		{
			res[cur++] = atimes + '0';
		}
		else
		{
			int bytes = 11;
			while (atimes > 0)
			{
				convbuf[--bytes] = (atimes % 10) + '0';
				atimes /= 10;
			}
			memcpy(res + cur, convbuf + bytes, 11 - bytes);
			cur += 11 - bytes;
		}
	}

	void append(const std::string& str)
	{
		memcpy(res + cur, str.data(), str.size());
		cur += str.size();
	}

	ContentLen size() const
	{
		return cur;
	}
};

}
}
