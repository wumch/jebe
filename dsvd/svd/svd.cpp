
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

#define _DSVD_CHECKABORT(invoker)		\
	ierr = invoker;						\
	CHKERRABORT(PETSC_COMM_WORLD, ierr);

namespace jebe {
namespace dsvd {
namespace svd {

void DSVD::run()
{
	prepare();
	build();
	solve();
	finalize();
}

void DSVD::prepare()
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
	_DSVD_CHECKABORT(SVDSolve(svd));

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

void DSVD::retrieve()
{
	prepare_retrieve();

	PetscReal s;
	Vec u, v;
	VecCreateSeq(PETSC_COMM_SELF, m, &u);
	VecCreateSeq(PETSC_COMM_SELF, n, &v);
	PetscInt i = 0;
	for (PetscInt end = nconved - 1; i < end; ++i)
	{
		_DSVD_CHECKABORT(SVDGetSingularTriplet(svd, i, &s, u, v));
		record(s, u ,v);
	}

	_DSVD_CHECKABORT(SVDGetSingularTriplet(svd, nconved - 1, &s, PETSC_NULL, PETSC_NULL));
	solution.related_error = std::sqrt(s);

	record_solution();
}

void DSVD::prepare_retrieve()
{
	_DSVD_CHECKABORT(SVDGetConverged(svd, &nconved));

	if (nconved <= 1)
	{
		MPI_Abort(PETSC_COMM_WORLD, 1);
		std::exit(1);
	}

	solution.r = r = nconved - 1;

	out_s = fopen(Aside::config->outfile_s.c_str(), "wb");
	mmap(NULL, r * sizeof(double), PROT_WRITE, MAP_PRIVATE, fileno(out_s), 0);

	out_u = fopen(Aside::config->outfile_u.c_str(), "wb");
	mmap(NULL, io_cache_elem * sizeof(double) * r, PROT_WRITE, MAP_PRIVATE, fileno(out_s), 0);

	out_v = fopen(Aside::config->outfile_v.c_str(), "wb");
	mmap(NULL, io_cache_elem * sizeof(double) * r, PROT_WRITE, MAP_PRIVATE, fileno(out_s), 0);

	out_solution = fopen(Aside::config->outfile_solution.c_str(), "wb");

	if (Aside::config->store_product)
	{
		MatCreateDense(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, m, r, PETSC_NULL, &U);
		MatCreateAIJ(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, r, 1, PETSC_NULL, 0, PETSC_NULL, &S);
		MatCreateDense(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, r, n, PETSC_NULL, &Vt);
	}
}

void DSVD::record(double s, Vec u, Vec v)
{
	fwrite(&s, sizeof(s), 1, out_s);

	static double* buf = new double[r];

	VecAssemblyBegin(u);
	VecAssemblyEnd(u);
	VecGetValues(u, r, 0, buf);
	fwrite(buf, sizeof(double), r, out_u);

	VecAssemblyBegin(v);
	VecAssemblyEnd(v);
	VecGetValues(v, r, 0, buf);
	fwrite(buf, sizeof(double), r, out_v);
}

void DSVD::record_solution()
{
	fwrite(&solution, sizeof(solution), 1, out_solution);
}

void DSVD::finalize()
{
	munmap(addr_s, io_cache_elem * sizeof(double));
	munmap(addr_u, io_cache_elem * (r * sizeof(double)));
	munmap(addr_v, io_cache_elem * (r * sizeof(double)));
	fclose(out_s);
	fclose(out_u);
	fclose(out_v);
	fclose(out_solution);
	_DSVD_CHECKABORT(SVDDestroy(&svd));
	_DSVD_CHECKABORT(MatDestroy(&A));
	_DSVD_CHECKABORT(SlepcFinalize());
}

DSVD::DSVD()
	: ierr(0), process_rank(0), is_main(false),
	  A(NULL), m(0), n(0), svd(NULL),
	  r(0), io_cache_elem(1 << 10),
	  out_u(NULL), addr_u(NULL),
	  out_s(NULL), addr_s(NULL),
	  out_v(NULL), addr_v(NULL),
	  out_solution(NULL)
{}

DSVD::~DSVD()
{
}

} /* namespace svd */
} /* namespace dsvd*/
} /* namespace jebe */

#undef _DSVD_CHECKABORT
