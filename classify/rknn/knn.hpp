
/**
Vector v;
SampleGroups sgs;

Knn knn(30);
knn.deliver(Aside::samples);
knn.classify(v, sgs);
 */

#pragma once

#include "predef.hpp"
#include <vector>
#include "vector.hpp"
#include "vecs.hpp"

namespace jebe {
namespace classify {
namespace rknn {

class Knn
{
public:
	Knn(vnum_t k_);

	clsid_t classify(const Vector& v, const SampleGroups& sgs)
	{
		reset();
		calc_top_k(v, sgs);
		return solve_best_cls();
	}

protected:
	void calc_top_k(const Vector& v, const SampleGroups& sgs);

	clsid_t solve_best_cls();

private:
	// reset `sim_top_k` and `cls_top_k`.
	void reset();

	// if need, record this (sim, cls) pair.
	void record(sim_t sim, clsid_t cls);
	// if need, record this (sim, cls) pair, via binary search.
	void record_bin(sim_t sim, clsid_t cls);
	// insert this (sim, cls) pair after idx.
	void record_after(vnum_t idx, sim_t sim, clsid_t cls);
	// insert this (sim, cls) pair at idx.
	void record_at(vnum_t idx, sim_t sim, clsid_t cls);
	// forward one step.
	void forward(vnum_t idx);

	// assign (sim, cls) pair.
	void assign(vnum_t idx, sim_t sim, clsid_t cls);

	void incr_cls_num(clsid_t cls);

private:
	void init();

	const vnum_t k;
	vnum_t recorded;

	static const sim_t miniest_sim;
	typedef std::vector<sim_t> SimTopK;
	SimTopK sim_top_k;

	typedef std::vector<clsid_t> ClsTopK;
	ClsTopK cls_top_k;

	typedef std::vector<vnum_t> ClsNumMap;
	ClsNumMap cls_num_map;
};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
