
#include "knn.hpp"
#include <iostream>
#include "vector.hpp"
#include "vector_impl.hpp"
#include "aside.hpp"

namespace jebe {
namespace classify {
namespace rknn {

const sim_t Knn::miniest_sim = -2.0;

inline void Knn::init()
{
	sim_top_k.resize(k);
	cls_top_k.resize(k);
	cls_num_map.resize(Aside::cls_num);
}

Knn::Knn(vnum_t k_)
	: k(k_)
{
	init();
}

inline void Knn::reset()
{
	recorded = 0;
	sim_top_k.assign(k, miniest_sim);
	cls_top_k.assign(k, SampleMeta::unknown_cls);
	cls_num_map.assign(cls_num_map.size(), 0);
}

inline void Knn::forward(vnum_t idx)
{
	for (vnum_t i = (recorded == k) ? (recorded - 1) : recorded++; i != idx; --i)
	{
		sim_top_k[i] = sim_top_k[i - 1];
	}
}

inline void Knn::assign(vnum_t idx, sim_t sim, clsid_t cls)
{
	sim_top_k[idx] = sim;
	cls_top_k[idx] = cls;
}

inline void Knn::record_after(vnum_t idx, sim_t sim, clsid_t cls)
{
	vnum_t i = (recorded == k) ? (recorded - 1) : recorded++;
	while (i != idx)
	{
		sim_top_k[i] = sim_top_k[i - 1];
		--i;
		if (sim_top_k[i] >= sim)
		{
			++i;
			break;
		}
	}
	assign(i, sim, cls);
}

inline void Knn::record_at(vnum_t idx, sim_t sim, clsid_t cls)
{
	forward(idx);
	assign(idx, sim, cls);
}

inline void Knn::record_bin(sim_t sim, clsid_t cls)
{
	if (sim <= sim_top_k[k - 1])
	{
		return;
	}
	int left = 0, right = k - 1, cur;
	while (left <= right)
	{
		cur = (left + right) >> 1;
		if (CS_BUNLIKELY(sim_top_k[cur] == sim))
		{
			record_after(cur, sim, cls);
		}
		else if (sim_top_k[cur] < sim)
		{
			right = cur - 1;
		}
		else
		{
			left = cur + 1;
		}
	}
	record_at(left, sim, cls);
}

inline void Knn::record(sim_t sim, clsid_t cls)
{
	record_bin(sim, cls);
}

inline void Knn::incr_cls_num(clsid_t cls)
{
	++cls_num_map[cls];
}

// solve the best class.
inline clsid_t Knn::solve_best_cls()
{
	for (ClsTopK::const_iterator it = cls_top_k.begin(); it != cls_top_k.end(); ++it)
	{
		incr_cls_num(*it);
	}
	clsid_t belong_cls;
	vnum_t vnum_max = 0;
	for (ClsNumMap::size_type i = 0; i < cls_num_map.size(); ++i)
	{
		if (cls_num_map[i] > vnum_max)
		{
			vnum_max = cls_num_map[i];
			belong_cls = i;
		}
	}
	return belong_cls;
}

inline void Knn::calc_top_k(const Vector& v, const SampleGroups& sgs)
{
	for (SampleGroups::iterator it = sgs.begin(); it != sgs.end(); ++it)
	{
		record(similarity(*it, v), it->belong_cls);
	}
}

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
