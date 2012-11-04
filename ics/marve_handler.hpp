
#pragma once

#include "predef.hpp"
#include <utility>
#include <boost/unordered_map.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <msgpack.hpp>
#include "handler.hpp"
#include "node.hpp"
#include "dictionary.hpp"
#include "seger.hpp"
#include "mbswcs.hpp"
#include "config.hpp"

namespace jebe {
namespace ics {

typedef msgpack::type::define<const std::string*, atimes_t> CWordWeightPacker;

class CWordWeight
	: public CWordWeightPacker
{
public:
	const std::string* word;
	atimes_t atimes;

public:
	explicit CWordWeight(const std::string* node_, atimes_t atimes_)
		: CWordWeightPacker(node_, atimes_),
		  word(node_), atimes(atimes_)
	{
	}

	CWordWeight& operator=(const CWordWeight& ww)
	{
		word = ww.word;
		atimes = ww.atimes;
		a0 = ww.a0;
		a1 = ww.a1;
		return *this;
	}

	template <typename Packer>
	void msgpack_pack(Packer& pk) const
	{
		pk.pack_array(2);
		pk.pack(*word);
		pk.pack(atimes);
	}
};

// hold words for calculate the marvelously-score.
class MarveHolder
{
private:
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

public:
	typedef boost::unordered_map<std::string, atimes_t> MapShadeType;
	typedef boost::fast_pool_allocator<MapShadeType::value_type,
		boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1024> WordsAllocType;
	typedef boost::unordered_map<std::string, atimes_t, MapShadeType::hasher, MapShadeType::key_equal, WordsAllocType> Words;

	typedef boost::fast_pool_allocator<CWordWeight,
		boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1024> OrderedWordsAllocType;
	typedef std::vector<CWordWeight> OrderedWords;

	Words words;

	static std::string empty_str;
	std::pair<std::string, atimes_t> word_atime;		// assit to avoid stack alloc.

	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer;

public:
	explicit MarveHolder():
		words(128),
		word_atime(empty_str, 1),
		packer(packerBuffer)
	{}

	void operator()(char* word, size_t word_len, const WordPOS& type)
	{
		weight_t weight = WordPOSCal::weight(type);
		if (weight != 0)
		{
			word_atime.first = std::string(word, word_len);
			Words::iterator it = words.find(std::string(word, word_len));
			if (it == words.end())
			{
				word_atime.second = weight;
				words.insert(word_atime);
			}
			else
			{
				it->second += weight;
			}
		}
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
			ow->push_back(CWordWeight(&it->first, it->second));
		}
		std::sort(ow->begin(), ow->end(), comparer);

		packerBuffer.clear();
		packer.pack(*ow);
		rep.rebuild(packerBuffer.data(), packerBuffer.size(), NULL, NULL);
	}

	void reset()
	{
		words.clear();
	}
};


class MarveHandler
	: public Handler
{
private:
	MarveHolder holder;

public:
	MarveHandler()
	{}

	virtual HandleRes process(char* content, tsize_t len, zmq::message_t& rep)
	{
		holder.reset();
		seger->process(content, len, holder);
		holder.genRes(rep);
		return success;
	}
};

}
}
