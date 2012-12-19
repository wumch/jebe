
#pragma once

#ifdef PETSC_USE_COMPLEX
#	error "This example does not work for complex numbers."
#endif

static const char help[] =
		"Reads in a Symmetric matrix in MatrixMarket format. Writes\n\
it using the PETSc sparse format. It also adds a Vector set to random values to the\n\
output file. Input parameters are:\n\
  -fin <filename> : input file\n\
  -fout <filename> : output file\n\n";

#include "predef.hpp"
#include <stdio.h>
#include <petscmat.h>

namespace jebe {
namespace dsvd {
namespace convert {

typedef struct {
	char infile[PETSC_MAX_PATH_LEN];
	char outfile[PETSC_MAX_PATH_LEN];
	PetscBool transpose;
	PetscBool daemon;
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

class Converter
{
private:
	int argc;
	char** argv;

	MPI_Comm comm_world;
	MPI_Comm comm_self;

	Param param;
	PetscErrorCode ierr;
	Stat stat;

	Mat A;
	Vec b;
	PetscViewer view;

	FILE* infile;

private:
	void fill()
	{
		get_stat();
		fill_mat();
		fill_vec();
	}

	void get_stat()
	{
		if (fread(&stat, sizeof(Stat), 1, infile) != 1)
		{
			exit(1);
		}
		printf("m = %lu, n = %lu, nnz = %lu\n", stat.total_rows, stat.total_cols, stat.nnz);
	}

	void fill_mat()
	{
		ierr = MatCreateSeqAIJ(comm_world, stat.total_rows, stat.total_cols, stat.nnz * 2 / stat.total_rows, 0, &A);
		CHKERRABORT(comm_world, ierr);

		ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE);
		CHKERRABORT(comm_world, ierr);

		int step = 1 << 10;
		Cell* cells = (Cell*)malloc(sizeof(Cell) * step);

		uint64_t cur = 0, i = 0, readed = 0;
		while (!feof(infile) && i < stat.nnz)
		{
			readed = fread(cells, sizeof(Cell), step, infile);
			for (i = 0; i < readed; ++i)
			{
				ierr = MatSetValue(A, cells[i].m, cells[i].n, cells[i].val, INSERT_VALUES);
				CHKERRABORT(comm_world, ierr);
			}
			cur += readed;
		}
		if (cur != stat.nnz)
		{
			printf("nnz:%lu, but got: %lu", stat.nnz, cur);
			exit(1);
		}
		fclose(infile);

		ierr = PetscPrintf(comm_self, "Reading matrix completes.\n");
		CHKERRABORT(comm_world, ierr);
	}

	void fill_vec()
	{
		ierr = VecCreate(comm_world, &b);
		CHKERRABORT(comm_world, ierr);

		ierr = VecSetSizes(b, PETSC_DECIDE, stat.total_cols);
		CHKERRABORT(comm_world, ierr);

		ierr = VecSetFromOptions(b);
		CHKERRABORT(comm_world, ierr);

		{
			PetscRandom r;
			ierr = PetscRandomCreate(comm_self, &r);
			CHKERRABORT(comm_world, ierr);

			ierr = PetscRandomSetFromOptions(r);
			CHKERRABORT(comm_world, ierr);

			ierr = VecSetRandom(b, r);
			CHKERRABORT(comm_world, ierr);
			ierr = PetscRandomDestroy(&r);
			CHKERRABORT(comm_world, ierr);
		}
	}

	void initialize()
	{
		PetscInitialize(&argc, &argv, (char*)PETSC_NULL, help);
		comm_world = PETSC_COMM_WORLD;
		comm_self = PETSC_COMM_SELF;

		if (argc < 5)
		{
			PetscPrintf(comm_world, "usage: %s -fin <input-matrix-file> -fout <output-matrix-file> [-transpose] [-daemon]\n", argv[0]);
			MPI_Abort(comm_world, 1);
		}

		PetscInt size;
		ierr = MPI_Comm_size(comm_world, &size);
		CHKERRABORT(comm_world, ierr);

		if (size > 1)
		{
			PetscPrintf(comm_world, "Uniprocessor Example only, but ur expecting run %d parallely\n", size);
			MPI_Abort(comm_world, 1);
		}

		{
			ierr = PetscOptionsGetBool(PETSC_NULL, "-daemon", &param.daemon, PETSC_NULL);
			CHKERRABORT(comm_world, ierr);
			if (param.daemon)
			{
				if (CS_BUNLIKELY(daemon(1, 0)))
				{
					MPI_Abort(comm_world, 1);
				}
			}
			ierr = PetscOptionsGetString(PETSC_NULL, "-fin", param.infile, PETSC_MAX_PATH_LEN, PETSC_NULL);
			CHKERRABORT(comm_world, ierr);
			ierr = PetscOptionsGetString(PETSC_NULL, "-fout", param.outfile, PETSC_MAX_PATH_LEN, PETSC_NULL);
			CHKERRABORT(comm_world, ierr);
			ierr = PetscOptionsGetBool(PETSC_NULL, "-transpose", &param.transpose, PETSC_NULL);
			CHKERRABORT(comm_world, ierr);
			ierr = PetscPrintf(comm_world, "will %s tranpose.\n", param.transpose ? "do" : "not");
			CHKERRABORT(comm_world, ierr);
		}
	}

	void prepare()
	{
		ierr = PetscFOpen(comm_self, param.infile, "rb", &infile);
		CHKERRABORT(comm_world, ierr);
	}

public:
	Converter(int argc, char* argv[])
		: argc(argc), argv(argv),
		  ierr(0)
	{}

	void run()
	{
		initialize();
		prepare();
		fill();
		dump();
	}

	void dump()
	{
		ierr = MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
		CHKERRABORT(comm_world, ierr);
		ierr = MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);
		CHKERRABORT(comm_world, ierr);

		ierr = PetscViewerBinaryOpen(comm_world, param.outfile, FILE_MODE_WRITE, &view);
		CHKERRABORT(comm_world, ierr);

		ierr = MatView(A, view);
		CHKERRABORT(comm_world, ierr);

		ierr = VecView(b, view);
		CHKERRABORT(comm_world, ierr);
	}

	~Converter()
	{
		ierr = PetscViewerDestroy(&view);
		CHKERRABORT(comm_world, ierr);

		ierr = VecDestroy(&b);
		CHKERRABORT(comm_world, ierr);

		ierr = MatDestroy(&A);
		CHKERRABORT(comm_world, ierr);

		ierr = PetscFinalize();
		CHKERRABORT(comm_world, ierr);
	}
};

}
}
}
