
#pragma once

#include "predef.hpp"
#include <utility>
#include <boost/unordered_map.hpp>
#include <msgpack.hpp>
#include "handler.hpp"
#include "node.hpp"

namespace jebe {
namespace cws {

typedef msgpack::type::define<const Node*, weight_t> WordWeightPacker;

class WordWeight
	: public WordWeightPacker
{
public:
	const Node* node;
	weight_t weight;

public:
	explicit WordWeight(const Node* node_, weight_t weight_)
		: WordWeightPacker(node_, weight_),
		  node(node_), weight(weight_)
	{
	}

	WordWeight& operator=(const WordWeight& ww)
	{
		node = ww.node;
		weight = ww.weight;
		a0 = ww.a0;
		a1 = ww.a1;
		return *this;
	}

	template <typename Packer>
	void msgpack_pack(Packer& pk) const
	{
		pk.pack_array(2);
		pk.pack(node->str());
		pk.pack(weight);
	}
};

class NodeHash
{
private:
	uint32_t mask;
public:
	uint32_t operator()(const Node* node) const
	{
		return (size_t)(node) & mask;
	}
};

// hold words for calculate the marvelously-score.
class MarveHolder
{
public:
	typedef boost::unordered_map<const Node*, atimes_t, NodeHash> Words;
	typedef std::vector<WordWeight> OrderedWords;
	Words words;
	tsize_t words_atimes_total;

	std::pair<const Node*, atimes_t> word_atime;		// assit to avoid stack alloc.
	WordWeight word_weight;		// assit to avoid stack alloc.

	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer;

public:
	explicit MarveHolder():
		words(128), words_atimes_total(0),
		word_atime(NULL, 1), word_weight(NULL, .0),
		packer(packerBuffer)
	{
	}

	void operator()(const Node* node)
	{
		Words::iterator it = words.find(node);
		if (CS_BUNLIKELY(it == words.end()))
		{
			word_atime.first = node;
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
		bool operator()(const WordWeight& w1, const WordWeight& w2) const
		{
			return w1.weight > w2.weight;
		}
	};
	MarveCompare comparer;

	void genRes(zmq::message_t& rep)
	{
		std::auto_ptr<OrderedWords> ow(new OrderedWords);
		ow->reserve(words.size());
		for (Words::const_iterator it = words.begin(); it != words.end(); ++it)
		{
			ow->push_back(WordWeight(it->first, (it->second * it->first->afreq() / words_atimes_total)));
		}
		std::sort(ow->begin(), ow->end(), comparer);

		packerBuffer.clear();
		packer.pack(*ow);
		rep.rebuild(packerBuffer.data(), packerBuffer.size(), NULL, NULL);
	}

	void reset()
	{
		words.clear();
		words_atimes_total = 0;
	}
};


class MarveHandler
	: public Handler
{
private:
	MarveHolder holder;
	char* resbuf;

public:
	MarveHandler()
		: holder()
	{
	}

	virtual HandleRes process(const byte_t* content, tsize_t len, zmq::message_t& rep)
	{
		holder.reset();
		filter->find(content, len, holder);
		holder.genRes(rep);
		return success;
	}

	virtual ~MarveHandler()
	{
		delete resbuf;
	}
};

}
}
