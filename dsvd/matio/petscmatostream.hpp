
#include "predef.hpp"
#include <petscmat.h>
#include "matostream.hpp"

namespace jebe {
namespace dsvd {
namespace matio {

/**
the format of PETSc::Mat:
$    int    MAT_FILE_CLASSID
$    int    number of rows
$    int    number of columns
$    int    total number of nonzeros
$    int    *number nonzeros in each row
$    int    *column indices of all nonzeros (starting index is zero)
$    PetscScalar *values of all nonzeros
*/

class PetscMatOStream: public MatOStream
{
private:
	Mat mat;

public:
	explicit PetscMatOStream(const char* path_);
	explicit PetscMatOStream(const char* path_, Mat mat_);
	explicit PetscMatOStream(const char* path_, size_type m_, size_type n_);
	explicit PetscMatOStream(const char* path_, size_type m_, size_type n_, Mat mat_);

	void bind(Mat mat_)
	{
		mat = mat_;
	}

protected:
	virtual void finalize();

	void store_mat();
	void store_mat(Mat mat_);
};

} /* namespace matio */
} /* namespace dsvd */
} /* namespace jebe */
