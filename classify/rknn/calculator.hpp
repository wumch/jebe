
#pragma once

#include "predef.hpp"
#include <vector>
#include "knn.hpp"
#include "vecs.hpp"

namespace jebe {
namespace classify {
namespace rknn {

// do KNN, with Rocchio optimization in <Calculator>.
class Calculator
{
private:
	static const int miniest_sim = -2;
public:
	Calculator()
		: knn(Aside::config->k),
		  cls_ids(Aside::config->ropt_cls_num, 0),
		  sim_top_n(Aside::config->ropt_cls_num, miniest_sim),
		  clses(*Aside::clses)
	{}

	clsid_t classify(const Vector& v)
	{
		pick_clses(v);
		return knn.classify(v, SampleGroups(*Aside::clses, cls_ids));
	}

private:
	void pick_clses(const Vector& v);

	void record_clsid(clsid_t clsid, sim_t sim);

private:
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


private:
	Knn knn;

	ClsIdList cls_ids;		// <Cls>.id of top similarity.
	typedef std::vector<sim_t> SimTopN;
	SimTopN sim_top_n;
	size_t recorded;

	const Clses& clses;
};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
