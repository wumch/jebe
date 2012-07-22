
#pragma once

#include "predef.hpp"
#include <boost/unordered_map.hpp>
#include "node.hpp"
#include "sendbuff.hpp"

namespace jebe {
namespace cws {

class Node;

// join pattens with space.
class SplitHolder
{
	SendBuff& res;
	tsize_t cur;
public:
	explicit SplitHolder(SendBuff& buff)
		: res(buff), cur(0)
	{
		CS_SAY("address of buff in SplitHolder: [" << &res << "]");
	}

	void operator()(const Node& node)
	{
		CS_SAY("address of buff operator(): [" << &res << "]");
		res.write(node.str().data(), node.str().size());
		res.write(' ');
//		memcpy(res + cur, node.str().data(), node.str().size());
//		cur += node.str().size();
//		res[cur++] = ' ';
	}

	void genRes() const
	{
		if (CS_BLIKELY(cur > 0))
		{
//			res[cur - 1] = 0;
		}
	}
};

class NodeHash
{
	uint32_t mask;
public:
	uint32_t operator()(const Node* const node) const
	{
		return (size_t)(node) & mask;
	}
};

class CompareHolder
{
public:
	typedef boost::unordered_map<const Node*, atimes_t, NodeHash> Words;
	Words words;
	SendBuff& res;

	std::pair<const Node*, atimes_t> word_atime;		// assit to make no stack alloc.

public:
	explicit CompareHolder(SendBuff& buff):
		words(128), res(buff), word_atime(NULL, 1)
	{
		res.write('{');
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

	void genRes()
	{

	}
};

// hold words and format to JSON.
class CountHolder
{
public:
	typedef boost::unordered_map<const Node*, atimes_t, NodeHash> Words;
	Words words;
	SendBuff& res;
	mutable tsize_t cur;
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
	explicit CountHolder(SendBuff& buff):
		words(128), res(buff), cur(1), word_atime(NULL, 1)
	{
		res.write('{');
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

	void genRes() const
	{
		CS_SAY(words.size());
		for (Words::const_iterator it = words.begin(); it != words.end(); ++it)
		{
			if (CS_BLIKELY(it != words.begin()))
			{
				res.write(',');
			}
			res.write('"');

			res.write(it->first->str().data(), it->first->str().size());
//			cur += it->first->str().size();

//			*reinterpret_cast<int16_t*>(res + cur) = mid_joiner;
			res.write(mid_joiner);
//			cur += sizeof(mid_joiner);

			res.insertNumber(it->second);
//			appendAtimes(it->second);
		}
		res.write(tail);
//		*reinterpret_cast<int16_t*>(res + cur) = tail;
//		cur += 1;
	}

//	void appendAtimes(atimes_t atimes) const
//	{
//		if (CS_BLIKELY(atimes < 10))
//		{
//			res[cur++] = atimes + '0';
//		}
//		else
//		{
//			int bytes = 11;
//			while (atimes > 0)
//			{
//				convbuf[--bytes] = (atimes % 10) + '0';
//				atimes /= 10;
//			}
//			memcpy(res + cur, convbuf + bytes, 11 - bytes);
//			cur += 11 - bytes;
//		}
//	}
//

	tsize_t size() const
	{
		return cur;
	}
};

}
}
