
#include "svd.hpp"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <sys/mman.h>
#include <boost/static_assert.hpp>
#include <petscversion.h>
#include <petscmat.h>
#include <slepcsvd.h>
#include "aside.hpp"

#define _DSVD_USV_DENSE 1

#if CS_DEBUG
#	define _DSVD_CHECKABORT(invoking)				\
	do {											\
		ierr = invoking;							\
		if (CS_UNLIKELY(ierr)) { CS_ERR("kid, PETSc error deteced by frank: " << __FILE__ << ":" << __LINE__ << "::" << #invoking << "=" << ierr); }	\
		CHKERRABORT(PETSC_COMM_WORLD, ierr);		\
	} while (false)
#else
#	define _DSVD_CHECKABORT(invoking)				\
		CHKERRABORT(PETSC_COMM_WORLD, invoking);
#endif

#if PETSC_VERSION_MINOR < 3
#	define _DSVD_ARG_FOR_DESTROY(var)	var
#else
#	define _DSVD_ARG_FOR_DESTROY(var)	&var
#endif

namespace jebe {
namespace dsvd {
namespace svd {

void DSVD::run()
{
	initiate();
	build();
	solve();
	finalize();
}

void DSVD::initiate()
{
	SlepcInitialize(const_cast<int*>(&Aside::config->argc),
		const_cast<char***>(&Aside::config->argv),
#if PETSC_VERSION_MINOR < 3
		const_cast<char*>(static_cast<const char*>(NULL)),
#else
		static_cast<const char*>(NULL),
#endif
		""
	);
	_DSVD_CHECKABORT(MPI_Comm_rank(PETSC_COMM_WORLD, &process_rank));
	is_main = process_rank == static_cast<typeof(process_rank)>(Aside::config->main_process);
}

void DSVD::build()
{
	// create A <Mat>.
	_DSVD_CHECKABORT(MatCreate(PETSC_COMM_WORLD, &A));
	_DSVD_CHECKABORT(MatSetFromOptions(A));

	// load A.
	PetscViewer viewer;
	_DSVD_CHECKABORT(PetscViewerBinaryOpen(PETSC_COMM_WORLD,
		Aside::config->matfile.string().c_str(), FILE_MODE_READ, &viewer));
	_DSVD_CHECKABORT(MatLoad(A, viewer));
	_DSVD_CHECKABORT(PetscViewerDestroy(_DSVD_ARG_FOR_DESTROY(viewer)));

	// create svd <SVD>.
	_DSVD_CHECKABORT(SVDCreate(PETSC_COMM_WORLD, &svd));

	_DSVD_CHECKABORT(SVDSetOperator(svd, A));

	_DSVD_CHECKABORT(SVDSetFromOptions(svd));

	// NOTE: if asc, then product is meaningless.
	if (Aside::config->asc)
	{
		_DSVD_CHECKABORT(SVDSetWhichSingularTriplets(svd, SVD_SMALLEST));
	}

	_DSVD_CHECKABORT(MatGetSize(A, &m, &n));
	PetscInt nsv = (Aside::config->nsv > 0)
		? std::min(std::min(m, n), static_cast<PetscInt>(Aside::config->nsv + 1))
		: PETSC_IGNORE;
	if (static_cast<int64_t>(Aside::config->nsv) > nsv)
	{
		LOG_IF(WARNING, Aside::config->loglevel > 0) << "expected nsv:" << Aside::config->ncv
			<< ", real nsv:" << nsv << " -- according to configure and real dataset.";
	}
	PetscInt ncv = (Aside::config->ncv > 0) ? Aside::config->ncv : PETSC_DECIDE;
	_DSVD_CHECKABORT(SVDSetDimensions(svd, nsv, ncv, PETSC_DECIDE));

	if (Aside::config->store_solution)
	{
		solution.origin_mat_m = m;
		solution.origin_mat_n = n;
		solution.nsv = nsv;
		solution.ncv = ncv;
	}
}

void DSVD::solve()
{
	decompose();
	if (is_main)
	{
		retrieve();
	}
}

void DSVD::decompose()
{
	// solve svd..
	if (is_main)
	{
		_DSVD_CHECKABORT(SVDSolve(svd));
	}

	{
		PetscInt iter_times;
		_DSVD_CHECKABORT(SVDGetIterationNumber(svd, &iter_times));
		LOG_IF(INFO, Aside::config->loglevel > 0) << "iterate times: " << iter_times;
	}

	{
		const SVDType svd_type;
		_DSVD_CHECKABORT(SVDGetType(svd, &svd_type));
		LOG_IF(INFO, Aside::config->loglevel > 0) << "solution method: " << svd_type;
	}

	{
		PetscInt nsv, max_iter;
		PetscReal tolerances;
		_DSVD_CHECKABORT(SVDGetDimensions(svd, &nsv, PETSC_NULL, PETSC_NULL));
		LOG_IF(INFO, Aside::config->loglevel > 0) << "nsv: " << nsv;

		_DSVD_CHECKABORT(SVDGetTolerances(svd, &tolerances, &max_iter));
		LOG_IF(INFO, Aside::config->loglevel > 0) << "stop condition: "
			"tolerances=" << tolerances << ","
			"max_iter=" << max_iter;
	}
}

void DSVD::prepare_retrieve() throw()
{
	_DSVD_CHECKABORT(SVDGetConverged(svd, &nconved));

	if (nconved <= 1)
	{
		CS_DIE_IF(MPI_Abort(PETSC_COMM_WORLD, 1) != MPI_SUCCESS, "failed on MPI_Abort()");
	}

	if (Aside::config->store_solution)
	{
		out_solution = std::fopen(Aside::config->outfile_solution.string().c_str(), "wb");
		out_solution_text = std::fopen(Aside::config->outfile_solution_text.string().c_str(), "wt");
		solution.r = r =  nconved - (static_cast<int64_t>(Aside::config->nsv) < nconved);
	}

	if (Aside::config->store_USV || Aside::config->store_USV_product)
	{
		_DSVD_CHECKABORT(MatCreateDense(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, m, r, PETSC_NULL, &U));
		_DSVD_CHECKABORT(MatCreateAIJ(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, r, 1, PETSC_NULL, 0, PETSC_NULL, &S));
		_DSVD_CHECKABORT(MatCreateDense(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, n, PETSC_NULL, &Vt));
	}
	if (Aside::config->store_product)
	{
#if _DSVD_USV_DENSE
		_DSVD_CHECKABORT(MatCreateDense(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, m, PETSC_NULL, &Ut));
		_DSVD_CHECKABORT(MatCreateAIJ(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, r, 1, PETSC_NULL, 0, PETSC_NULL, &Sv));
#endif
	}
}

// retrieve results.
void DSVD::retrieve()
{
	prepare_retrieve();

	// retrieve singular triplets.
	PetscReal s;
	Vec u, v;
	_DSVD_CHECKABORT(VecCreateSeq(PETSC_COMM_SELF, m, &u));
	_DSVD_CHECKABORT(VecCreateSeq(PETSC_COMM_SELF, n, &v));
	for (PetscInt i = 0; i < r; ++i)
	{
		_DSVD_CHECKABORT(SVDGetSingularTriplet(svd, i, &s, u, v));
		record(i, s, u ,v);
	}

	if (Aside::config->store_USV || Aside::config->store_USV_product)
	{
		_DSVD_CHECKABORT(MatAssemblyBegin(U, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(U, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyBegin(S, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(S, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyBegin(Vt, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(Vt, MAT_FINAL_ASSEMBLY));
	}
	if (Aside::config->store_product)
	{
		_DSVD_CHECKABORT(MatAssemblyBegin(Ut, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(Ut, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyBegin(Sv, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(Sv, MAT_FINAL_ASSEMBLY));
	}

	if (static_cast<int64_t>(Aside::config->nsv) < nconved)
	{
		_DSVD_CHECKABORT(SVDGetSingularTriplet(svd, nconved - 1, &s, PETSC_NULL, PETSC_NULL));
	}

	store_USV();

	if (Aside::config->store_solution)
	{
		solution.related_error = (static_cast<int64_t>(Aside::config->nsv) < nconved) ? std::sqrt(s) : 0;
		record_solution();
	}

	if (Aside::config->store_product)
	{
		calc_SvUt();
		calc_SvUtA();
	}
}

void DSVD::store_USV()
{
	if (Aside::config->store_USV_product)
	{
		Mat US, US_dense, USV;
		MatMatMult(U, S, MAT_INITIAL_MATRIX, PETSC_DEFAULT, &US);
		MatConvert(US, MATSEQDENSE, MAT_INITIAL_MATRIX, &US_dense);
		free(US);
		MatMatMult(US_dense, Vt, MAT_INITIAL_MATRIX, PETSC_DEFAULT, &USV);
		free(US_dense);
		frozen_mat(Aside::config->outfile_USV_product, USV);
	}

	if (Aside::config->store_USV)
	{
		frozen_mat(Aside::config->outfile_U, U);
		frozen_mat(Aside::config->outfile_S, S);
		frozen_mat(Aside::config->outfile_Vt, Vt);
	}
}

// (Sk ^ -1) * transposed(Uk).
// NOTE: it will change S => 1/S. so, should store S (even if need not S) before product.
void DSVD::calc_SvUt()
{
	MatMatMult(Sv, Ut, MAT_INITIAL_MATRIX, PETSC_DEFAULT, &SvUt);
	store_mat(Aside::config->outfile_SvUt, SvUt);
}

void DSVD::calc_SvUtA()
{
	MatMatMult(SvUt, A, MAT_INITIAL_MATRIX, PETSC_DEFAULT, &SvUtA);
	frozen_mat(Aside::config->outfile_feature_space, SvUtA);
	free(SvUt);
}

void DSVD::record(PetscInt turn, double s, Vec u, Vec v)
{
	if (Aside::config->store_product || Aside::config->store_USV)
	{
		record(turn, s, u, v, r);
	}
}

void DSVD::record(PetscInt turn, double s, Vec u, Vec v, PetscInt size)
{
	// S
	if (Aside::config->store_USV || Aside::config->store_USV_product)
	{
		_DSVD_CHECKABORT(MatSetValues(S, 1, &turn, 1, &turn, &s, INSERT_VALUES));
	}
	if (Aside::config->store_product)
	{
		const double s_inv = 1.0 / static_cast<long double>(s);
		_DSVD_CHECKABORT(MatSetValues(Sv, 1, &turn, 1, &turn, &s_inv, INSERT_VALUES));
	}

	static PetscScalar* const buf = new PetscScalar[size];
	static PetscInt* const idxm = new PetscInt[size];
	static PetscInt* const idxn = new PetscInt[size];
	static bool idxn_inited = false;
	if (!idxn_inited)
	{
		idxn_inited = true;
		for (PetscInt i = 0; i < size; ++i)
		{
			idxn[i] = i;
		}
	}
	for (PetscInt i = 0; i < size; ++i)
	{
		idxm[i] = turn;
	}

	// U
	VecAssemblyBegin(u);
	VecAssemblyEnd(u);
	VecGetValues(u, size, idxn, buf);
	if (Aside::config->store_USV || Aside::config->store_USV_product)
	{
		MatSetValues(U, size, idxn, 1, idxm, buf, INSERT_VALUES);
	}
	if (Aside::config->store_product)
	{
		MatSetValues(Ut, 1, idxm, size, idxn, buf, INSERT_VALUES);
	}

	// Vt
	if (Aside::config->store_USV || Aside::config->store_USV_product)
	{
		VecAssemblyBegin(v);
		VecAssemblyEnd(v);
		VecGetValues(v, size, idxn, buf);
		MatSetValues(Vt, 1, idxm, size, idxn, buf, INSERT_VALUES);
	}
}

void DSVD::record_solution()
{
	CS_DIE_IF(static_cast<PetscInt>(std::fwrite(&solution, sizeof(solution), 1, out_solution)) != 1, "fwrite not completely done");
	CS_DIE_IF(std::fputs(pack_solution(solution).c_str(), out_solution_text) == EOF, "fputs not completely done");
}

void DSVD::finalize()
{
	// so that this function is thread-safe and idenpotent(projective).
	if (finalized == 0 && __sync_add_and_fetch(&finalized, 1) == 1)
	{
		if (Aside::config->store_USV)
		{
			CS_DIE_IF(std::fclose(out_solution) != 0, "fclose failed");
		}
		_DSVD_CHECKABORT(SVDDestroy(_DSVD_ARG_FOR_DESTROY(svd)));
		free(A);
		_DSVD_CHECKABORT(SlepcFinalize());
	}
}

DSVD::DSVD()
	:
#if CS_DEBUG
	  ierr(0),
#endif
	  process_rank(0), is_main(false),
	  A(NULL), m(0), n(0), svd(NULL),
	  r(0), io_cache_elem(1 << 10),
	  out_solution(NULL),
	  finalized(0)
{}

void DSVD::frozen_mat(const boost::filesystem::path& filename, Mat mat)
{
	frozen_mat(filename.string().c_str(), mat);
}

void DSVD::frozen_mat(const char* filename, Mat mat)
{
	store_mat(filename, mat);
	free(mat);
}

void DSVD::store_mat(const boost::filesystem::path& filename, Mat mat)
{
	store_mat(filename.string().c_str(), mat);
}

void DSVD::store_mat(const char* filename, Mat mat)
{
	PetscViewer viewer;
	_DSVD_CHECKABORT(PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_WRITE, &viewer));
	store_mat(mat, viewer);
}

void DSVD::store_mat(Mat mat, PetscViewer viewer, PetscInt mat_n)
{
	if (mat_n == mat_n_unknown)
	{
		_DSVD_CHECKABORT(MatGetSize(mat, PETSC_NULL, &mat_n));
	}

	{
		_DSVD_CHECKABORT(MatAssemblyBegin(mat, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(mat, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatView(mat, viewer));
	}

	Vec vec;
	{
		_DSVD_CHECKABORT(VecCreateSeq(PETSC_COMM_SELF, mat_n, &vec));
		_DSVD_CHECKABORT(VecSetFromOptions(vec));

		{
			PetscRandom rand;
			_DSVD_CHECKABORT(PetscRandomCreate(PETSC_COMM_SELF, &rand));
			_DSVD_CHECKABORT(PetscRandomSetFromOptions(rand));
			_DSVD_CHECKABORT(VecSetRandom(vec, rand));
			_DSVD_CHECKABORT(PetscRandomDestroy(_DSVD_ARG_FOR_DESTROY(rand)));
		}

		_DSVD_CHECKABORT(VecView(vec, viewer));
	}

	_DSVD_CHECKABORT(PetscViewerDestroy(_DSVD_ARG_FOR_DESTROY(viewer)));
	_DSVD_CHECKABORT(VecDestroy(_DSVD_ARG_FOR_DESTROY(vec)));
}

CS_FORCE_INLINE void DSVD::free(Mat& mat_)
{
	_DSVD_CHECKABORT(MatDestroy(_DSVD_ARG_FOR_DESTROY(mat_)));
	mat_ = NULL;
}

CS_FORCE_INLINE void DSVD::free(Vec& vec_)
{
	VecDestroy(_DSVD_ARG_FOR_DESTROY(vec_));
	vec_ = NULL;
}
DSVD::~DSVD()
{
	finalize();
}

} /* namespace svd */
} /* namespace dsvd*/
} /* namespace jebe */

#undef _DSVD_CHECKABORT
#undef _DSVD_ARG_FOR_DESTROY
