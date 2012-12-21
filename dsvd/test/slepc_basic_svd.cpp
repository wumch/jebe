
static char help[] = "Standard symmetric eigenproblem corresponding to the Laplacian operator in 1 dimension.\n\n"
"The command line options are:\n"
" -n <n>, where <n> = number of grid subdivisions = matrix dimension.\n\n";

//#include <iostream>
#include <slepc.h>

int main(int argc, char* argv[])
{
	SlepcInitialize(&argc, &argv, (char*)0, help);

	SVD svd;
	Mat A;
	Vec u, v;
	PetscReal sigma;

	PetscInt num;

	PetscReal error;

	SVDCreate(PETSC_COMM_WORLD, &svd);
	SVDSetOperator(svd, A);
	SVDSetFromOptions(svd);
	SVDSolve(svd);
	SVDGetConverged(svd, &num);

	for (PetscInt i = 0; i < num; ++i)
	{
		SVDGetSingularTriplet(svd, i, &sigma, u, v);
		SVDComputeRelativeError(svd, i, &error);
	}
	SVDDestroy(&svd);
}
