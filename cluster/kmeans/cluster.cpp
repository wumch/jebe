
#include "cluster.hpp"
#include <boost/numeric/ublas/io.hpp>
#include "aside.hpp"
#include "utils.hpp"
#include "vector_impl.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

Cluster::Cluster(clsid_t id_)
	: id(id_), count(0)
{}

Cluster::Cluster(clsid_t id_, const Vector& center_)
	: id(id_), count(0), center(center_)
{}

Cluster::Cluster(clsid_t id_, const Vector* center_)
	: id(id_), count(0), center(*center_)
{}

void Cluster::attach(const Vector* vec)
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

void Cluster::recalc_center()
{
	RawVector rv(Aside::totalFeatureNum, guess_nonzeros(size()));
	for (MemberList::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		rv += ***it;
	}
	center.reset(rv / size());
}

void Cluster::clear_members()
{
	count = 0;
	members.clear();
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
