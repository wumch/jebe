
#pragma once

#include "predef.hpp"
#include <stdio.h>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <petscmat.h>
#include "../solution.h"

namespace jebe {
namespace dsvd {
namespace svd {

typedef size_t size_type;
typedef double scalar_type;
BOOST_STATIC_ASSERT(sizeof(size_type) >= 4 && sizeof(scalar_type) >= 4);

#pragma pack(4)
typedef struct
{
	size_type m, n, nnz;
#ifdef __cplusplus
public:
	CS_FORCE_INLINE void reset(size_type m_, size_type n_, size_type nnz_)
	{
		m = m_;
		n = n_;
		nnz = nnz_;
	}
#endif
} MatHeader;

typedef struct
{
	size_type m, n;
	scalar_type val;
#ifdef __cplusplus
public:
	CS_FORCE_INLINE void reset(size_type m_, size_type n_, scalar_type val_)
	{
		m = m_;
		n = n_;
		val = val_;
	}
#endif
} MatCell;
#pragma pack()

class MatOStream: private boost::noncopyable
{
private:
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

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
