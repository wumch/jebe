
#include "predef.hpp"

#ifdef __cplusplus
namespace jebe {
namespace dsvd {
namespace matio {
#endif

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
#pragma pack()


#pragma pack(4)
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

#ifdef __cplusplus
}
}
}
#endif
