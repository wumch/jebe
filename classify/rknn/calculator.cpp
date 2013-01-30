
#include "calculator.hpp"
#include "vector_impl.hpp"

namespace jebe {
namespace classify {
namespace rknn {

inline void Calculator::assign(vnum_t idx, sim_t sim, clsid_t cls)
{
	sim_top_n[idx] = sim;
	cls_ids[idx] = cls;
}

inline void Calculator::record_after(vnum_t idx, sim_t sim, clsid_t cls)
{
	vnum_t i = (recorded == Aside::config->ropt_cls_num) ? (recorded - 1) : recorded++;
	while (i != idx)
	{
		sim_top_n[i] = sim_top_n[i - 1];
		--i;
		if (sim_top_n[i] >= sim)
		{
			++i;
			break;
		}
	}
	assign(i, sim, cls);
}

inline void Calculator::forward(vnum_t idx)
{
	for (vnum_t i = (recorded == cls_ids.size()) ? (recorded - 1) : recorded++; i != idx; --i)
	{
		sim_top_n[i] = sim_top_n[i - 1];
	}
}

inline void Calculator::record_at(vnum_t idx, sim_t sim, clsid_t cls)
{
	forward(idx);
	assign(idx, sim, cls);
}

inline void Calculator::record_bin(sim_t sim, clsid_t cls)
{
	if (sim <= sim_top_n[Aside::config->ropt_cls_num - 1])
	{
		return;
	}
	int left = 0, right = Aside::config->ropt_cls_num - 1, cur;
	while (left <= right)
	{
		cur = (left + right) >> 1;
		if (CS_BUNLIKELY(sim_top_n[cur] == sim))
		{
			record_after(cur, sim, cls);
		}
		else if (sim_top_n[cur] < sim)
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

void Calculator::pick_clses(const Vector& v)
{
	for (Clses::ClsList::const_iterator it = clses->begin(); it != clses->end(); ++it)
	{
		record_clsid(it->id, similarity(it->ropt, v));
	}
}

void Calculator::record_clsid(clsid_t clsid, sim_t sim)
{
	record_bin(sim, clsid);
}

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
