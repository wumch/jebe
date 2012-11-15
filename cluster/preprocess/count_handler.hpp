
#pragma once

#include "predef.hpp"
#include <utility>
#include <boost/unordered_map.hpp>
#include <msgpack.hpp>
#include "handler.hpp"
#include "node.hpp"

namespace jebe {
namespace idf {

typedef msgpack::type::define<const Node*, atimes_t> CWordWeightPacker;

class CWordWeight
	: public CWordWeightPacker
{
public:
	const Node* node;
	atimes_t atimes;

public:
	explicit CWordWeight(const Node* node_, atimes_t atimes_)
		: CWordWeightPacker(node_, atimes_),
		  node(node_), atimes(atimes_)
	{
	}

	CWordWeight& operator=(const CWordWeight& ww)
	{
		node = ww.node;
		atimes = ww.atimes;
		a0 = ww.a0;
		a1 = ww.a1;
		return *this;
	}

	template <typename Packer>
	void msgpack_pack(Packer& pk) const
	{
		pk.pack_array(2);
		pk.pack(node->str());
		pk.pack(atimes);
	}
};

// hold words for calculate the marvelously-score.
class CountHolder
{
public:
	typedef boost::unordered_map<const Node*, atimes_t, NodeHash> Words;
	typedef std::vector<CWordWeight> OrderedWords;
	Words words;
	tsize_t words_atimes_total;

	std::pair<const Node*, atimes_t> word_atime;		// assit to avoid stack alloc.
	CWordWeight word_weight;		// assit to avoid stack alloc.

	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer;

public:
	explicit CountHolder():
		words(128), words_atimes_total(0),
		word_atime(NULL, 1), word_weight(NULL, .0),
		packer(packerBuffer)
	{}

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
		bool operator()(const CWordWeight& w1, const CWordWeight& w2) const
		{
			return w1.atimes > w2.atimes;
		}
	};
	MarveCompare comparer;

	void genRes(zmq::message_t& rep)
	{
		std::auto_ptr<OrderedWords> ow(new OrderedWords);
		ow->reserve(words.size());
		for (Words::const_iterator it = words.begin(); it != words.end(); ++it)
		{
			ow->push_back(CWordWeight(it->first, it->second));
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


class CountHandler
	: public Handler
{
private:
	CountHolder holder;
	char* resbuf;

public:
	CountHandler()
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

	virtual ~CountHandler()
	{
		delete resbuf;
	}
};

}
}
