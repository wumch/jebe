
#include "matstream.hpp"
#include <cstddef>

namespace jebe {
namespace dsvd {
namespace svd {

MatOStream::MatOStream(const char* path_)
	: nnz(0),
	  path(path_), fp(NULL),
	  started(0), finished(0)
{
	header.reset(0, 0, nnz);
	cell.reset(0, 0, 0);
}

MatOStream::MatOStream(const char* path_, size_type m_, size_type n_)
	: nnz(0),
	  path(path_), fp(NULL),
	  started(0), finished(0)
{
	header.reset(m_, n_, nnz);
	cell.reset(0, 0, 0);
	reset_size(m_, n_);
}

void MatOStream::start()
{
	if (started == 0 && __sync_add_and_fetch(&started, 1) == 1)
	{
		CS_ABORT_IF(!(fp = fopen(path, "wb")));
	}
}

void MatOStream::finish()
{
	if (finished == 0 && __sync_add_and_fetch(&finished, 1) == 1)
	{
		write_nnz();
		CS_ABORT_IF(fclose(fp) != 0);
	}
}

void MatOStream::append(size_type m, size_type n, scalar_type val)
{
	set(m, n, val);
	++nnz;
}

// reset
void MatOStream::reset(const MatHeader& header_)
{
	header = header_;
	write_head();
}

void MatOStream::reset(size_type m, size_type n, size_type nnz)
{
	header.reset(m, n, nnz);
	write_head();
}

void MatOStream::reset_rows(size_type m)
{
	header.m = m;
}

void MatOStream::reset_cols(size_type n)
{
	header.n = n;
}

void MatOStream::reset_nnz(size_type nnz_)
{
	header.nnz = nnz = nnz_;
	write_nnz();
}

void MatOStream::reset_size(size_type m, size_type n)
{
	reset_rows(m);
	reset_cols(n);
}

void MatOStream::write_head()
{
	bool seeked = false;
	long cur = ftell(fp);
	if (CS_BLIKELY(cur > 0))
	{
		CS_ABORT_IF(fseek(fp, 0, SEEK_SET) != 0);
	}
	else if (CS_BUNLIKELY(cur == 0))
	{
		CS_DIE("kid, error occured in ftell() at" << __FILE__ << ":" << __LINE__ << ", error=" << errno << ":" << strerror(errno));
	}
	write(header);
	if (seeked)
	{
		fseek(fp, cur, SEEK_SET);
	}
}

void MatOStream::write_nnz()
{
	CS_ABORT_IF(fseek(fp, offsetof(MatHeader, nnz), SEEK_SET) != 0);
	write(nnz);
}

MatOStream::~MatOStream()
{
	finish();
}

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
