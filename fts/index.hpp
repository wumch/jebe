
#pragma once

#include "predef.hpp"
#include <string>
#include <utility>
#include <boost/unordered_map.hpp>
#include <boost/pool/pool_alloc.hpp>
#include "hash.hpp"
#include "config.hpp"

namespace jebe {
namespace fts {

class Index
{
private:
	static const Index* const instance;
public:
	// would better call this in bootstrap period.
	CS_FORCE_INLINE static const Index* getInstance()
	{
		return instance;
	}

public:
	typedef boost::unordered_map<docid_t, marve_t> DocWeightMap;
protected:
	mutable DocWeightMap dwmap;

protected:
	typedef boost::fast_pool_allocator<std::string,
			boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 		// currently it's readonly
			JEBE_FTS_WORDS_HASH_BITS
		> MapAllocType;		// as <key> is std::string, this alloctor is useless.

public:
	typedef boost::unordered_map<std::string, DocWeightList, staging::JSHash,
			std::equal_to<std::string>, MapAllocType> MapType;

protected:
	MapType map;

public:
	Index();

public:
	void match(const WordWeightList& words, std::vector<docid_t>& docids, std::size_t max_match = 0) const;

};

} /* namespace fts */
} /* namespace jebe */
