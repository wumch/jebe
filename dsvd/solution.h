
#pragma once

#ifdef __cplusplus
#	include <string>
#	include <sstream>
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
	int32_t nsv;			// expected "nsv"
	int32_t ncv;
	uint32_t r;				// closed "rank", actually calculated "nsv".
	double related_error;	// 谱范数算术根
} Solution;
#pragma pack()

#ifdef __cplusplus
#	define _DSVD_PRETY_PROPERTY(name)	<< #name << ":\t" << s.name << CS_LINESEP
static inline std::string pack_solution(const Solution& s)
{
	std::stringstream stream;
	stream
		_DSVD_PRETY_PROPERTY(origin_mat_m)
		_DSVD_PRETY_PROPERTY(origin_mat_n)
		_DSVD_PRETY_PROPERTY(nsv)
		_DSVD_PRETY_PROPERTY(ncv)
		_DSVD_PRETY_PROPERTY(r)
		_DSVD_PRETY_PROPERTY(related_error)
	;
	return stream.str();
}
#	undef _DSVD_PRETY_PROPERTY
#endif

#ifdef __cplusplus
} /* namespace dsvd*/
} /* namespace jebe */
#endif
