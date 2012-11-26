
#pragma once

#include "predef.hpp"
#include <memory>
#include <fstream>
#include <stdio.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include "autoincr.hpp"
#include "aside.hpp"
#include "cluster.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

class Calculater
{
private:
	typedef staging::AutoIncr<staging::AutoIncrType<Cluster>, clsid_t, 1> ClsIdGen;
	std::auto_ptr<ClsIdGen> cls_id_gen;

private:
	VecList& vecs;

	typedef boost::ptr_vector<Cluster> ClsList;
	ClsList clses;

	uint32_t level;
	uint32_t supposed_k;
	uint32_t k;

	uint64_t min_members;
	uint64_t max_members;

	uint32_t iter_times;

	uint32_t cls_idx_offset;

	mutable struct DeliverCursor
	{
		decimal_t min_disp;
		uint32_t cls_idx;
		decimal_t cur_disp;
	} dcur;

	bool any_change;

	FILE* centers_out;
	std::ofstream cls_vecs_out;

public:
	Calculater();
	~Calculater();

	void calcu();

private:
	void prepare();

	void pick_centers();

	void pass();

	bool enough();

	void deliver();
	void deliver(const Vector& vec);
	void deliver(const Vector& vec, Cluster& cls);

	Cluster& get_cls_by_id(clsid_t clsid);

	void recalc_centers();

private:
	bool should_optimize();

	void optimize();
	ClsList::iterator optimize(ClsList::iterator cls_iter);

	// 分裂
	ClsList::iterator separate(ClsList::iterator cls_iter);
	void separate(Cluster& cls);

	// 分解
	ClsList::iterator decompose(ClsList::iterator cls_iter);
	void decompose(Cluster::MemberList& members);

public:
	void dump();

	void upward();

	bool reached_top();

	void regen_bound();

	void reset_supposed_k(clsnum_t sk_);

public:
	bool validate();		// validate parameters, dataset.
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
