
#pragma once

#include "predef.hpp"
#include <stdio.h>
#include <boost/noncopyable.hpp>
#include "component.hpp"

namespace jebe {
namespace dsvd {
namespace matio {

class MatOStream: private boost::noncopyable
{
protected:
	MatHeader header;

	size_type nnz;

	mutable MatCell cell;	// assist to avoid from frequent alloc.

	const char* path;

	FILE* fp;

	int started;
	int finished;

public:
	explicit MatOStream(const char* path_);
	explicit MatOStream(const char* path_, size_type m_, size_type n_);

	virtual ~MatOStream();

	void start();

	void finish();

public:
	CS_FORCE_INLINE void reset(const MatHeader& header_);
	CS_FORCE_INLINE void reset(size_type m, size_type n, size_type nnz);
	CS_FORCE_INLINE void reset_rows(size_type m);
	CS_FORCE_INLINE void reset_cols(size_type n);
	CS_FORCE_INLINE void reset_nnz(size_type nnz);
	CS_FORCE_INLINE void reset_size(size_type m, size_type n);

	CS_FORCE_INLINE void append(size_type m, size_type n, scalar_type val);

protected:
	void write_head();

	void write_nnz();

	virtual void finalize();

protected:
	CS_FORCE_INLINE void set(size_type m, size_type n, scalar_type val)
	{
		cell.reset(m, n, val);
		write(cell);
	}

	template<typename ElementType>
	CS_FORCE_INLINE void write(ElementType element)
	{
		CS_ABORT_IF(fwrite(&element, sizeof(ElementType), 1, fp) != 1);
	}
};

} /* namespace matio */
} /* namespace dsvd */
} /* namespace jebe */
