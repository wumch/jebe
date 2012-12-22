
#pragma once

#ifndef __GNUC__
#	error "this file require typeof(**ptr) support."
#endif

#include "predef.hpp"
#include <vector>
#include <stdio.h>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <petscmat.h>
#include <slepcsvd.h>
#include "../solution.h"

namespace {
	typedef typeof(**(new Vec)) VecClassType;
}

namespace jebe {
namespace dsvd {
namespace svd {

class DSVD: private boost::noncopyable
{
public:
	DSVD();

	void prepare();

	void run();

	~DSVD();

private:
	PetscErrorCode ierr;

	PetscMPIInt process_rank;
	bool is_main;

private:
	typedef boost::ptr_vector<VecClassType> VecList;
	typedef std::vector<PetscReal> ScalarList;

	Solution solution;

	Mat A;
	PetscInt m;
	PetscInt n;

	SVD svd;
	Mat U;
	Mat S;
	Mat Vt;

	PetscInt r;
	PetscInt nconved;
	size_t io_cache_elem;

	FILE* out_u;
	char* addr_u;
	FILE* out_s;
	char* addr_s;
	FILE* out_v;
	char* addr_v;
	FILE* out_solution;
	FILE* out_solution_text;

private:
	void build();

	void solve();

	void finalize();

private:
	void decompose();

	void prepare_retrieve();
	void retrieve();

	void CS_FORCE_INLINE record(PetscReal s, Vec u, Vec v);
	void record(double s, Vec u, Vec v, size_t size);

	void record_solution();
};

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
