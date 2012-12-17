#if !defined(PETSC_USE_COMPLEX)

static char help[] =
		"Reads in a Symmetric matrix in MatrixMarket format. Writes\n\
it using the PETSc sparse format. It also adds a Vector set to random values to the\n\
output file. Input parameters are:\n\
  -fin <filename> : input file\n\
  -fout <filename> : output file\n\n";

#include <petscmat.h>



typedef struct {
	char infile[PETSC_MAX_PATH_LEN];
	char outfile[PETSC_MAX_PATH_LEN];
	PetscBool transpose;
} Param;

#pragma pack(4)
typedef struct {
	uint64_t total_rows;
	uint64_t total_cols;
	uint64_t nnz;
} Stat;
#pragma pack()

#pragma pack(4)
typedef struct {
	uint32_t m;
	uint32_t n;
	double val;
} Cell;
#pragma pack()

PetscErrorCode ierr;

Mat fill(const Param* param)
{
	FILE* file;
	ierr = PetscFOpen(PETSC_COMM_SELF, param->infile, "rb", &file);
	CHKERRABORT(PETSC_COMM_WORLD, ierr);

	Stat stat;
	if (fread(&stat, sizeof(Stat), 1, file) != 1)
	{
		exit(1);
	}

	printf("m = %lu, n = %lu, nnz = %lu\n", stat.total_rows, stat.total_cols, stat.nnz);

	Mat A;
	ierr = MatCreateSeqAIJ(PETSC_COMM_WORLD, stat.total_rows, stat.total_cols, stat.nnz * 2 / stat.total_rows, 0, &A);
	CHKERRABORT(PETSC_COMM_WORLD, ierr);

	ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE);
	CHKERRABORT(PETSC_COMM_WORLD, ierr);

	int step = 1 << 10;
	Cell* buf = (Cell*)malloc(sizeof(Cell) * step);

	uint64_t cur = 0, i = 0, readed = 0;
	while (!feof(file) && i < stat.nnz)
	{
		readed = fread(buf, sizeof(Cell), step, file);
		for (i = 0; i < readed; ++i)
		{
			ierr = MatSetValue(A, buf[i].m, buf[i].n, buf[i].val, INSERT_VALUES);
			CHKERRABORT(PETSC_COMM_WORLD, ierr);
		}
		cur += readed;
	}
	if (cur != stat.nnz)
	{
		printf("nnz:%lu, but got: %lu", stat.nnz, cur);
		exit(1);
	}
	fclose(file);

	ierr = MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
	CHKERRABORT(PETSC_COMM_WORLD, ierr);

	ierr = MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);
	CHKERRABORT(PETSC_COMM_WORLD, ierr);

	return A;
}


#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char **args)
{
	Vec b;
	char buf[PETSC_MAX_PATH_LEN];
	PetscInt m, n, nnz;
	PetscErrorCode ierr;
	PetscMPIInt size;
	PetscViewer view;
	PetscRandom r;

	PetscInitialize(&argc, &args, (char *)PETSC_NULL, help);

	ierr = MPI_Comm_size(PETSC_COMM_WORLD, &size);
	CHKERRQ(ierr);

	if (size > 1)
	{
		SETERRQ(PETSC_COMM_WORLD, 1, "Uniprocessor Example only\n");
	}

	Param param;
	{
		/* Read in matrix and RHS */
		ierr = PetscOptionsGetString(PETSC_NULL, "-fin", param.infile, PETSC_MAX_PATH_LEN, PETSC_NULL);
		CHKERRQ(ierr);
		ierr = PetscOptionsGetString(PETSC_NULL, "-fout", param.outfile, PETSC_MAX_PATH_LEN, PETSC_NULL);
		CHKERRQ(ierr);
		ierr = PetscOptionsGetBool(PETSC_NULL, "-transpose", &param.transpose, PETSC_NULL);
		CHKERRQ(ierr);
		ierr = PetscPrintf(PETSC_COMM_WORLD, "will %s tranpose.\n", param.transpose ? "do" : "not");
		CHKERRQ(ierr);
	}

	/* process header with comments */
//	do
//	{
//		fgets(buf, PETSC_MAX_PATH_LEN - 1, file);
//	}
//	while (buf[0] == '%');


	Mat A = fill(&param);

//	int got;
//	/* The first non-comment line has the matrix dimensions */
//	got = sscanf(buf, "%d %d %d\n", &m, &n, &nnz);
//	if (__builtin_expect(got != 3, 0))
//	{
//		printf("num of got by sscanf is not expected:%d/%d", got, 3);
//		return 1;
//	}

//	swap_int_if(transpose, &m, &n);
	ierr = VecCreate(PETSC_COMM_WORLD, &b);
	CHKERRQ(ierr);

	ierr = VecSetSizes(b, PETSC_DECIDE, n);
	CHKERRQ(ierr);

	ierr = VecSetFromOptions(b);
	CHKERRQ(ierr);

	ierr = PetscRandomCreate(PETSC_COMM_SELF, &r);
	CHKERRQ(ierr);

	ierr = PetscRandomSetFromOptions(r);
	CHKERRQ(ierr);

	ierr = VecSetRandom(b, r);
	CHKERRQ(ierr);

	ierr = MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
	CHKERRQ(ierr);

	ierr = MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);
	CHKERRQ(ierr);

	ierr = PetscPrintf(PETSC_COMM_SELF, "Reading matrix completes.\n");
	CHKERRQ(ierr);

	if (param.transpose)
	{
		Mat At;
		ierr = MatCreateSeqAIJ(PETSC_COMM_WORLD, n, m, nnz * 2 / n, 0, &At);
		CHKERRQ(ierr);
		ierr = PetscPrintf(PETSC_COMM_SELF, "At created.\n");
		CHKERRQ(ierr);
		ierr = MatSetOption(At, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE);
		CHKERRQ(ierr);
		ierr = PetscPrintf(PETSC_COMM_SELF, "Option settled.\n");
		CHKERRQ(ierr);
		ierr = MatTranspose(A, MAT_INITIAL_MATRIX, &At);
		CHKERRQ(ierr);
		ierr = PetscPrintf(PETSC_COMM_SELF, "transposed.\n");
		CHKERRQ(ierr);

		ierr = MatDestroy(&A);
		CHKERRQ(ierr);

		A = At;
	}

	ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD, param.outfile, FILE_MODE_WRITE, &view);
	CHKERRQ(ierr);

	ierr = MatView(A, view);
	CHKERRQ(ierr);

	ierr = VecView(b, view);
	CHKERRQ(ierr);

	ierr = PetscViewerDestroy(&view);
	CHKERRQ(ierr);

	ierr = VecDestroy(&b);
	CHKERRQ(ierr);

	ierr = MatDestroy(&A);
	CHKERRQ(ierr);

	ierr = PetscRandomDestroy(&r);
	CHKERRQ(ierr);

	ierr = PetscFinalize();
	return 0;
}
#else
#include <stdio.h>
int main(int argc,char **args)
{
	fprintf(stdout,"This example does not work for complex numbers.\n");
	return 0;
}
#endif
