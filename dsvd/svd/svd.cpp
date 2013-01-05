
#include "svd.hpp"
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <sys/mman.h>
#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>
#include <petscmat.h>
#include <slepcsvd.h>
#include "aside.hpp"

#define _DSVD_USV_DENSE 1

#if CS_DEBUG > 1
#	define _DSVD_CHECKABORT(invoking)				\
	do {											\
		ierr = invoking;							\
		if (CS_UNLIKELY(ierr)) { CS_ERR("kid, PETSc error deteced by frank: " << __FILE__ << ":" << __LINE__ << "::" << #invoking << "=" << ierr); }	\
		CHKERRABORT(PETSC_COMM_WORLD, ierr);		\
	} while (false)
#else
#	define _DSVD_CHECKABORT(invoking)				\
	do {											\
		ierr = invoking;							\
		CHKERRABORT(PETSC_COMM_WORLD, ierr);		\
	} while (false)
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
		static_cast<const char*>(NULL), ""
	);
	_DSVD_CHECKABORT(MPI_Comm_rank(PETSC_COMM_WORLD, &process_rank));
//	BOOST_STATIC_ASSERT(boost::type_traits::is_same<typeof(process_rank), typeof(Aside::config->main_process)>::value);
	is_main = process_rank == Aside::config->main_process;
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
	_DSVD_CHECKABORT(PetscViewerDestroy(&viewer));

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
			"tolerances" << tolerances << ","
			"max_iter=" << max_iter;
	}
}

void DSVD::prepare_retrieve() throw()
{
	_DSVD_CHECKABORT(SVDGetConverged(svd, &nconved));

	if (nconved <= 1)
	{
		if (MPI_Abort(PETSC_COMM_WORLD, 1) != MPI_SUCCESS)
		{
			std::exit(CS_EXIT_STATUS_FAILED);
			throw new int(1);
		}
	}

	if (Aside::config->store_solution)
	{
		out_solution = fopen(Aside::config->outfile_solution.c_str(), "wb");
		out_solution_text = fopen(Aside::config->outfile_solution_text.c_str(), "w");
		solution.r = r = nconved - 1;
	}

	if (Aside::config->store_USV)
	{
		out_S = fopen(Aside::config->outfile_S.c_str(), "wb");
//		mmap(NULL, r * sizeof(double), PROT_WRITE, MAP_PRIVATE, fileno(out_s), 0);

		out_U = fopen(Aside::config->outfile_U.c_str(), "wb");
//		mmap(NULL, io_cache_elem * sizeof(double) * r, PROT_WRITE, MAP_PRIVATE, fileno(out_s), 0);

		out_Vt = fopen(Aside::config->outfile_Vt.c_str(), "wb");
//		mmap(NULL, io_cache_elem * sizeof(double) * r, PROT_WRITE, MAP_PRIVATE, fileno(out_s), 0);
	}

	if (Aside::config->store_product)
	{
#if _DSVD_USV_DENSE
		_DSVD_CHECKABORT(MatCreateDense(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, m, r, PETSC_NULL, &U));
		_DSVD_CHECKABORT(MatCreateAIJ(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, r, 1, PETSC_NULL, 0, PETSC_NULL, &S));
		_DSVD_CHECKABORT(MatCreateDense(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, n, PETSC_NULL, &Vt));
#endif
		_DSVD_CHECKABORT(VecCreateSeq(PETSC_COMM_WORLD, r, &s_inverse));
	}
}

//
void DSVD::retrieve()
{
	prepare_retrieve();

	// retrieve singular triplets.
	PetscReal s;
	Vec u, v;
	_DSVD_CHECKABORT(VecCreateSeq(PETSC_COMM_SELF, m, &u));
	_DSVD_CHECKABORT(VecCreateSeq(PETSC_COMM_SELF, n, &v));
	for (PetscInt i = 0, end = nconved - 1; i < end; ++i)
	{
		_DSVD_CHECKABORT(SVDGetSingularTriplet(svd, i, &s, u, v));
		record(i, s, u ,v);
	}
	if (Aside::config->store_product)
	{
		_DSVD_CHECKABORT(MatAssemblyBegin(U, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(U, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyBegin(Vt, MAT_FINAL_ASSEMBLY));
		_DSVD_CHECKABORT(MatAssemblyEnd(Vt, MAT_FINAL_ASSEMBLY));
	}

	_DSVD_CHECKABORT(SVDGetSingularTriplet(svd, nconved - 1, &s, PETSC_NULL, PETSC_NULL));

	if (Aside::config->store_solution)
	{
		solution.related_error = std::sqrt(s);
		record_solution();
	}

	if (Aside::config->store_product)
	{
		product();
		record_product();
	}
}

// (Sk ^ -1) * transposed(Uk).
// NOTE: it will change S => 1/S. so, should store S (even if need not S) before product.
void DSVD::product()
{
	_DSVD_CHECKABORT(MatDiagonalSet(S, s_inverse, INSERT_VALUES));
	Mat P;
	MatMatMult(S, U, MAT_INITIAL_MATRIX, PETSC_DEFAULT, &P);
}

void DSVD::record_product()
{

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
	if (Aside::config->store_USV)
	{
		CS_ABORT_IF(static_cast<PetscInt>(fwrite(&s, sizeof(s), 1, out_S)) != 1);
	}
	if (Aside::config->store_product)
	{
		const double s_inv = 1.0 / static_cast<long double>(s);
		_DSVD_CHECKABORT(VecSetValues(s_inverse, 1, &turn, &s_inv, INSERT_VALUES));
	}

	static double* const buf = new double[size];
	static PetscInt* const idxm = new PetscInt[size];
	static PetscInt* const idxn = new PetscInt[size];
	static bool idx_inited = false;
	if (!idx_inited)
	{
		idx_inited = true;
		for (PetscInt i = 0; i < size; ++i)
		{
			idxm[i] = turn;
			idxn[i] = i;
		}
	}

	// U
	VecAssemblyBegin(u);
	VecAssemblyEnd(u);
	VecGetValues(u, size, idxn, buf);
	if (Aside::config->store_USV)
	{
		CS_ABORT_IF(static_cast<PetscInt>(fwrite(buf, sizeof(double), size, out_U)) != size);
	}
	if (Aside::config->store_product)
	{
		MatSetValues(U, 1, idxm, size, idxn, buf, INSERT_VALUES);
	}

	// V
	VecAssemblyBegin(v);
	VecAssemblyEnd(v);
	VecGetValues(v, size, idxn, buf);
	if (Aside::config->store_USV)
	{
		CS_ABORT_IF(static_cast<PetscInt>(fwrite(buf, sizeof(double), size, out_Vt)) != size);
	}
	if (Aside::config->store_product)
	{
		MatSetValues(Vt, size, idxn, 1, idxm, buf, INSERT_VALUES);
	}
}

void DSVD::record_solution()
{
	CS_ABORT_IF(static_cast<PetscInt>(fwrite(&solution, sizeof(solution), 1, out_solution)) != 1);
	std::string solution_text = pack_solution(solution);
	CS_ABORT_IF(fputs(solution_text.c_str(), out_solution_text) != static_cast<int>(solution_text.size()));
}

void DSVD::finalize()
{
	// so that this function is thread-safe and idenpotent(projective).
	if (finalized == 0 && __sync_add_and_fetch(&finalized, 1) == 1)
	{
		if (Aside::config->store_USV)
		{
//			munmap(addr_s, io_cache_elem * sizeof(double));
//			munmap(addr_u, io_cache_elem * (r * sizeof(double)));
//			munmap(addr_v, io_cache_elem * (r * sizeof(double)));
			fclose(out_S);
			fclose(out_U);
			fclose(out_Vt);
			fclose(out_solution);
		}
		_DSVD_CHECKABORT(SVDDestroy(&svd));
		_DSVD_CHECKABORT(MatDestroy(&A));
		_DSVD_CHECKABORT(SlepcFinalize());
	}
}

DSVD::DSVD()
	: ierr(0), process_rank(0), is_main(false),
	  A(NULL), m(0), n(0), svd(NULL),
	  r(0), io_cache_elem(1 << 10),
	  out_U(NULL), addr_U(NULL),
	  out_S(NULL), addr_S(NULL),
	  out_Vt(NULL), addr_Vt(NULL),
	  out_solution(NULL),
	  finalized(0)
{}

DSVD::~DSVD()
{
	finalize();
}

} /* namespace svd */
} /* namespace dsvd*/
} /* namespace jebe */

#undef _DSVD_CHECKABORT
