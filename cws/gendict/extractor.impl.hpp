
#pragma once

#include "staging.hpp"
#include "extractor.hpp"
#include "misc.hpp"
#include <boost/preprocessor.hpp>


#if CS_IS_LITTLE_ENDIAN
#	define _JEBE_STR_SHT_2(bit)			BOOST_PP_IF(BOOST_PP_MOD(bit, 2), ^, _JEBE_STR_SHT_4(bit))
#else
#	define _JEBE_STR_SHT_2(bit)			BOOST_PP_IF(BOOST_PP_MOD(bit, 2), _JEBE_STR_SHT_4(bit), ^)
#endif
#define _JEBE_STR_SHT_EXPR(bit)			(reinterpret_cast<const uint16_t*>(p.str)[bit])
#define _JEBE_STR_SHT_4(bit)			BOOST_PP_IF(BOOST_PP_MOD(bit, 4), _JEBE_STR_SHT_EXPR(bit), (_JEBE_STR_SHT_EXPR(bit) << 9))
#define _JEBE_STR_SHT_R(z, bit, d)		_JEBE_STR_SHT_2(bit)
#define _JEBE_STR_SHT(bit)				BOOST_PP_REPEAT_FROM_TO(0, BOOST_PP_DEC(BOOST_PP_MUL(bit, 2)), _JEBE_STR_SHT_R, BOOST_PP_EMPTY())

#define BOOST_PP_LOCAL_MACRO(n)			\
	template<>	CS_FORCE_INLINE uint32_t hfhash(const Phrase<n>& p) { return _JEBE_STR_SHT(n); }
#define BOOST_PP_LOCAL_LIMITS			(1, BOOST_PP_ADD(_JEBE_MAP_MAX_LEN, 2))

namespace jebe {
namespace cws {

template<uint8_t plen> CS_FORCE_INLINE uint32_t hfhash(const Phrase<plen>& p);

#include BOOST_PP_LOCAL_ITERATE()

}
}
