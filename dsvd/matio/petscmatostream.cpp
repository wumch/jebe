
#include "petscmatostream.hpp"

namespace jebe {
namespace dsvd {
namespace matio {

PetscMatOStream::PetscMatOStream(const char* path_)
	: MatOStream(path_), mat(NULL)
{}

PetscMatOStream::PetscMatOStream(const char* path_, Mat mat_)
	: MatOStream(path_), mat(mat_)
{}

PetscMatOStream::PetscMatOStream(const char* path_, size_type m_, size_type n_)
	: MatOStream(path_, m_, n_), mat(NULL)
{}

PetscMatOStream::PetscMatOStream(const char* path_, size_type m_, size_type n_, Mat mat_)
	: MatOStream(path_, m_, n_), mat(mat_)
{}

void PetscMatOStream::finalize()
{
	CS_RETURN_IF(mat == NULL);
	store_mat(mat);
}

void PetscMatOStream::store_mat()
{
	PetscInt* m, * n;
	MatGetSize(mat, m, n);
	reset_size(*m, *n);

	MatInfo info;
	MatGetInfo(mat, MAT_GLOBAL_SUM, &info);
	reset_nnz(info.nz_allocated);

//	MatGetValues(mat, 0);
}

void PetscMatOStream::store_mat(Mat mat_)
{
	bind(mat_);
	store_mat();
}

} /* namespace matio */
} /* namespace dsvd */
} /* namespace jebe */
