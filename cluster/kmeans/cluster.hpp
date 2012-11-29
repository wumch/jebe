
#pragma once

#include "predef.hpp"
#include <list>
#include <boost/noncopyable.hpp>
#include <boost/pool/pool_alloc.hpp>
#include "utils.hpp"
#include "aside.hpp"
#include "vector.hpp"
#include "vector_impl.hpp"

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
	Cluster(clsid_t id_)
		: id(id_), count(0)
	{}

	Cluster(clsid_t id_, const Vector& center_)
		: id(id_), count(0), center(center_)
	{}

	Cluster(clsid_t id_, const Vector* center_)
		: id(id_), count(0), center(*center_)
	{}

	void clear_members()
	{
		count = 0;
		members.clear();
	}

	void attach(const Vector* vec)
	{
		members.push_back(vec);
		++count;
#if _JEBE_ENABLE_AUTO_RECALC
		if (CS_BUNLIKELY(count < Aside::config->auto_recalc_threshold))
		{
			recalc_center();
		}
#endif
	}

	void recalc_center()
	{
		RawVector rv(Aside::totalFeatureNum, guess_nonzeros(size()));
		for (MemberList::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			rv += ***it;
		}
		center.reset(rv / size());
	}

	vnum_t size() const
	{
		return count;
	}
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
