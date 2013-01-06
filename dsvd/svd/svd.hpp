
#pragma once

#ifndef __GNUC__
#	error "this file require typeof(**ptr) support."
#endif

#include "predef.hpp"
#include <cstdio>
#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <petscmat.h>
#include <slepcsvd.h>
#include "../solution.h"

namespace {
	typedef typeof(**(new Vec)) VecClassType;
}

namespace jebe {
namespace dsvd {
namespace svd {

/**
 * clause:
 * 1. first letter of matrix name must be capital anywhere, including global, member, local variables.
 * 2. symbol idiom (for both matrix and vector):
 * 		operator '*' indicates dot-product (only in comments),
 * 		subscript 'v' indicates "inversed",
 * 		subscript 't' indicates "transposed",
 * 		subscript 'k' indicates "cutted (no matter by row or by column)".
 */
/**
 * 约定:
 * 1. 任何地方矩阵变量名首字母都必须大写，包括全局、成员、本地变量.
 * 2. 符号惯用法（针对矩阵和向量）:
 * 		操作符 '*' 表示 点积（只在注释里）,
 * 		下标 'v' 表示 逆,
 * 		下标 't' 表示 转置,
 * 		下标 'k' 表示 截断（不论按行还是按列）。
 */
class DSVD: private boost::noncopyable
{
public:
	DSVD();

	void run();

	~DSVD();

private:
#if CS_DEBUG
	PetscErrorCode ierr;
#endif

	PetscMPIInt process_rank;	// mpi-rank of current process.
	bool is_main;				// whether current process is the "main-process" of the session.
								// some jobs are for only main-process, such as store results into disk.
private:
	Solution solution;	// info about the result of solve.

	Mat A;				// problem matrix. m * n dim.
	PetscInt m;
	PetscInt n;

	SVD svd;			// solver. A = U * S * Vt.
	Mat U;
	Mat S;
	Mat Vt;

	Mat Ut;				// transposed U.
	Mat Sv;				// inversed S. as E/S.
	Mat SvUt;			// (Sk ^ -1) * transposed(Uk).
	Mat SvUtA;				// SvUt * A, the feature space.

	PetscInt r;				// expected rank. indicate how many triplesets should be computed and retrieved.
	PetscInt nconved;		// how many triplesets are really computed.
	size_t io_cache_elem;	// how many vectors(rows) should be cached while mmap.

//	FILE* out_U;			// fp of output-file U.
//	char* addr_U;			// mmap addr of output-file U.
//	FILE* out_S;
//	char* addr_S;
//	FILE* out_Vt;
//	char* addr_Vt;
	std::FILE* out_solution;		// fp of output-file solution -- binary format.
	std::FILE* out_solution_text;	// fp of output-file solution -- plain text format.

	int finalized;		// flag indicates that finalized (all done and all of resouce are released) or not.

private:
	void initiate();

	void build();

	void solve();

	void finalize();	// thread-safe and idenpotent finish solving.

private:
	void decompose();

	void prepare_retrieve() throw();
	void retrieve();

	void record(PetscInt turn, PetscReal s, Vec u, Vec v);
	void record(PetscInt turn, double s, Vec u, Vec v, PetscInt size);

	void record_solution();

	void store_USV();

	void calc_SvUt();	// product SvUt, the transformation.
	void calc_SvUtA();	// product SvUt, the feature_space.

private:	// IO related.
	void store_mat(const boost::filesystem::path& filename, Mat mat);
	void store_mat(const char* filename, Mat mat);
	void store_mat(Mat mat, PetscViewer viewer);
	void frozen_mat(const boost::filesystem::path& filename, Mat mat);
	void frozen_mat(const char* filename, Mat mat);		// store, then destroy matrix.

private:	// utility
	CS_FORCE_INLINE void free(Mat& mat_);
	CS_FORCE_INLINE void free(Vec& vec_);
};

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
