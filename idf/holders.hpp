
#pragma once

#include "predef.hpp"
#include <boost/unordered_map.hpp>
#include <vector>
#include <auto_ptr.h>
#include "node.hpp"

namespace jebe {
namespace idf {

class Node;

// join pattens with space.
class SplitHolder
{
	SendBuff& res;
	tsize_t cur;
public:
	explicit SplitHolder(SendBuff& buff)
		: res(buff)
	{
		CS_SAY("address of buff in SplitHolder: [" << &res << "]");
	}

	void operator()(const Node& node)
	{
		CS_SAY("address of buff operator(): [" << &res << "]");
		res.write(node.str().data(), node.str().size());
		res.write(' ');
	}

	void genRes() const
	{
		if (CS_BLIKELY(!res.empty()))
		{
			res.backspace(0);
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

			res.write(mid_joiner);

			res.insertNumber(it->second);
		}
		res.write(tail);
	}

	tsize_t size() const
	{
		return cur;
	}
};

// hold words for calculate marvelously-score.
class MarveHolder
{
public:
	typedef boost::unordered_map<const Node*, atimes_t, NodeHash> Words;
	typedef std::pair<const Node*, double> Word;
	typedef std::vector<Word> OrderedWords;
	Words words;
	SendBuff& res;
	mutable tsize_t cur;
	tsize_t words_atimes_total;
	static const int16_t mid_joiner =
#if CS_IS_LITTLE_ENDIAN
			(',' << CHAR_BIT) + '"';
#else
			('"' << CHAR_BIT) + ',';
#endif
			static const int32_t end_joiner =		// ],["
#if CS_IS_LITTLE_ENDIAN
					('"' << (CHAR_BIT * 3)) + ('[' << (CHAR_BIT * 2)) + (',' << CHAR_BIT) + ']';
#else
					('"' << CHAR_BIT) + ',';
#endif
	static const int16_t tail =
#if CS_IS_LITTLE_ENDIAN
			(']' << CHAR_BIT) + ']';
#else
			']' << CHAR_BIT;
#endif

	std::pair<const Node*, atimes_t> word_atime;		// assit to avoid stack alloc.
	mutable char convbuf[11];

public:
	explicit MarveHolder(SendBuff& buff):
		words(128), res(buff), cur(1), words_atimes_total(0), word_atime(NULL, 1)
	{
		res.write('[');
		res.write('[');
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
		++words_atimes_total;
	}

	class MarveCompare
	{
	public:
		bool operator()(const Word& w1, const Word& w2) const
		{
			return w1.second > w2.second;
		}
	};

	void genRes() const
	{
		std::auto_ptr<OrderedWords> ow(new OrderedWords);
		ow->reserve(words.size());
		for (Words::const_iterator it = words.begin(); it != words.end(); ++it)
		{
			CS_SAY("afreq of [" << it->first << "] :" << it->first->afreq());
			ow->push_back(std::make_pair(it->first, (static_cast<double>(it->second) / words_atimes_total) / it->first->afreq()));
		}
		std::sort(ow->begin(), ow->end(), MarveCompare());


		CS_SAY(words.size());
		std::string score;
		for (OrderedWords::const_iterator it = ow->begin(); it != ow->end(); ++it)
		{
			if (CS_BLIKELY(it != ow->begin()))
			{
				if (CS_BLIKELY(it != ow->end()))
				{
					res.write(end_joiner);
				}
				else
				{
					res.write(*reinterpret_cast<const uint16_t*>("]]"));
				}
			}
			else
			{
				res.write('"');
			}

			res.write(it->first->str().data(), it->first->str().size());

			res.write(mid_joiner);

			score = boost::lexical_cast<std::string>(it->second);
			CS_SAY("it->second: " << it->second << ", score: " << score);
			res.write(score.data(), score.size());

//			res.insertNumber(it->second);
		}
		res.write(tail);
	}

	tsize_t size() const
	{
		return cur;
	}
};

}
}
