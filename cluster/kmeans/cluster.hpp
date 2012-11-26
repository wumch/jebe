
#pragma once

#include "predef.hpp"
#include <list>
#include <boost/noncopyable.hpp>
#include <boost/pool/pool_alloc.hpp>
#include "vector.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

class Cluster
	: public boost::noncopyable
{
public:
	typedef boost::fast_pool_allocator<const Vector*,
		boost::default_user_allocator_malloc_free, boost::details::pool::default_mutex, 10000> MemberAllocType;
	typedef std::list<const Vector*, MemberAllocType> MemberList;

public:
	clsid_t id;
	uint32_t level;

	MemberList members;
	vnum_t count;

	Vector center;

public:
	Cluster(clsid_t id_);

	Cluster(clsid_t id_, const Vector& center_);

	Cluster(clsid_t id_, const Vector* center_);

	void clear_members();

	void attach(const Vector* vec);

	void recalc_center();

	vnum_t size() const
	{
		return count;
	}
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
