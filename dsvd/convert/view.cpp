
/**
 * view a PETSc matrix.
 * g++ -I/data/fsuggest/staging/ccpp view.cpp -o view
 */

#include "predef.hpp"
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include "filesystem.hpp"
#include "net.hpp"

namespace jebe {
namespace dsvd {
namespace convert {

//$    int    MAT_FILE_CLASSID
//$    int    number of rows
//$    int    number of columns
//$    int    total number of nonzeros
//$    int    *number nonzeros in each row
//$    int    *column indices of all nonzeros (starting index is zero)
//$    PetscScalar *values of all nonzeros

class View
{
private:
	typedef double PetscScalar;

	const char* const infile_name;
	std::FILE* infile;

	const char* const outfile_name;
	std::FILE* outfile;

	int total_m, total_n, total_nnz;

	bool isprint;

public:
	View(const char* infile_name_, const char* outfile_name_ = NULL)
		: infile_name(infile_name_), outfile_name(outfile_name_),
		  total_m(0), total_n(0), total_nnz(0), isprint(!outfile_name)
	{
		CS_DIE_IF((infile = std::fopen(infile_name, "rb")) == NULL, "fopen " << infile_name << " failed");
		if (outfile_name)
		{
			CS_DIE_IF((outfile = std::fopen(outfile_name, "wt")) == NULL, "fopen " << outfile_name << " failed");
		}
		else
		{
			outfile = stdout;
		}
	}

	void dump()
	{
		dump_header();
		dump_cells();
	}

private:
	void dump_header()
	{
		int buf[4];
		fread(buf, sizeof(int), 4, infile);
		total_m = ntohl(buf[1]);
		total_n = ntohl(buf[2]);
		total_nnz = ntohl(buf[3]);
		fprintf(stdout, "MAT_FILE_CLASSID: %d\nm=%d, n=%d, nnz=%d\n", ntohl(buf[0]), total_m, total_n, total_nnz);
	}

	void dump_cells()
	{
		int* const nnz_of_rows = new int[total_m];
		int* const nz_indices = new int[total_nnz];
		PetscScalar* const nz_values = new PetscScalar[total_n];
		PetscScalar* const values = new PetscScalar[total_n];

		CS_DIE_IF(fread(nnz_of_rows, sizeof(int), total_m, infile) != total_m, "fread failed");
		CS_DIE_IF(fread(nz_indices, sizeof(int), total_nnz, infile) != total_nnz, "fread failed");

		if (isprint)
		{
			for (int i = 0; i <= total_n; ++i)
			{
				if (i == 0)
				{
					fprintf(outfile, (isprint ? "\033[32;31;5m" "%4s  " "\033[0m" : "%4s  "), "0");
				}
				else
				{
					fprintf(outfile, (isprint ? "\033[32;49;5m" "%-13d" "\033[0m" : "%-10d"), i);
				}
			}
			fprintf(outfile, "\n");
		}

		PetscScalar val;
		int next_nz_index, next_cell_j_idx, nnz_idx_offset = 0;
		for (int curm = 0, cur_nnz; curm < total_m; ++curm)
		{
			if (isprint)
			{
				fprintf(outfile, (isprint ? "\033[32;49;5m" "%4d  " "\033[0m" : "%4d  "), curm + 1);
			}
			cur_nnz = ntohl(nnz_of_rows[curm]);

			CS_DIE_IF(fread(nz_values, sizeof(PetscScalar), cur_nnz, infile) != cur_nnz, "fread failed");

			memset(values, 0, total_n * sizeof(PetscScalar));
			uint64_t ival;
			double val;
			for (int i = 0; i < cur_nnz; ++i)
			{
				values[ntohl(nz_indices[nnz_idx_offset + i])] = staging::ntoh(nz_values[i]);
			}
			nnz_idx_offset += cur_nnz;
			for (int i = 0; i < total_n; ++i)
			{
				if (values[i] == 0)
				{
					fprintf(outfile, isprint ? "%-13s" : "%s\t", isprint ? "-" : "0");
				}
				else
				{
					fprintf(outfile, (isprint ? "\033[32;34;5m" "%-13.8lf" "\033[0m" : "%lf\t"), values[i]);
				}
			}
			fprintf(outfile, "\n");
		}

		size_t fpos = ftell(infile), fsize = staging::filesize(infile_name);
		if (fpos != fsize)
		{
			fprintf(stderr, "\033[32;31;5m" "WARN: file end not reached. %ld/%ld\n" "\033[0m", fpos, fsize);
		}
	}
};

}
}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "usage: %s <PETSc-matrix-file> [output-file]\n", argv[0]);
		std::exit(1);
	}
	using namespace jebe::dsvd::convert;
	View* viewer = new View(argv[1], argc > 2 ? argv[2] : NULL);
	viewer->dump();
}

