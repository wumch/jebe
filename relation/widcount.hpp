
#pragma once

#include "predef.hpp"
#include <msgpack.hpp>

namespace jebe {
namespace rel {

// for msgpack/unpck.
template<typename Tag, typename id_t, typename count_t>
class IdCount
{
public:
	id_t id;
	count_t count;

public:
	IdCount() {}

	explicit IdCount(id_t wordid_, count_t atime)
		: id(wordid_), count(atime)
	{
	}

	MSGPACK_DEFINE(id, count);
};

class WIdCountTag {};
typedef IdCount<WIdCountTag, wordid_t, wnum_t> WIdCount;

class DIdCountTag {};
typedef IdCount<DIdCountTag, docid_t, docnum_t> DIdCount;

template<typename Tag, typename id_t, typename count_t>
static inline bool operator==(const IdCount<Tag, id_t, count_t>& id_count_1, const IdCount<Tag, id_t, count_t>& id_count_2)
{
	return id_count_1.id == id_count_2.id;
}

template<typename Tag, typename id_t, typename count_t>
static inline bool operator<(const IdCount<Tag, id_t, count_t>& id_count_1, const IdCount<Tag, id_t, count_t>& id_count_2)
{
	return id_count_1.id < id_count_2.id;
}

}
}
