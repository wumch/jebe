
#pragma once

#ifdef __cplusplus
#	include "predef.hpp"
#else
#	include <stdint.h>
#endif

#ifdef __cplusplus
namespace jebe {
namespace dsvd {
#endif

#pragma pack(4)
typedef struct
{
	uint64_t origin_mat_m;
	uint64_t origin_mat_n;
	uint32_t nsv;			// expected "nsv"
	uint32_t ncv;
	uint32_t r;				// closed "rank", actually calculated "nsv".
	double related_error;	// 谱范数算术根
} Solution;
#pragma pack()

#ifdef __cplusplus
} /* namespace dsvd*/
} /* namespace jebe */
#endif
