
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
#define BOOST_PP_LOCAL_LIMITS			(1, BOOST_PP_ADD(_JEBE_WORD_MAX_LEN, 2))

namespace jebe {
namespace cws {

BOOST_STATIC_ASSERT(sizeof(CharType) == 4);

template<uint8_t plen> CS_FORCE_INLINE uint32_t hfhash(const Phrase<plen>& p)
{
	uint32_t mask = 0;
	char* res = reinterpret_cast<char*>(&mask);
	const char* str_ = reinterpret_cast<const char*>(p.str);
	for (uint8_t i = 0, end = plen * sizeof(CharType); i < end; ++i)
	{
		res[i & 3] ^= str_[i];
	}
	return mask;
}

#include BOOST_PP_LOCAL_ITERATE()

template<uint8_t len_1, uint8_t len_2>
CS_FORCE_INLINE bool match(const CharType prefix[len_1], const CharType rstr[len_2])
{
	BOOST_STATIC_ASSERT(len_1 <= len_2);
	switch (len_1)
	{
	case 1:
		return prefix[0] == rstr[0];
	case 2:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1];
	case 3:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2];
	case 4:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3];
	case 5:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4];
	case 6:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4]
			 && prefix[5] == rstr[5];
	case 7:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4]
			 && prefix[5] == rstr[5]
			 && prefix[6] == rstr[6];
	default:
		for (uint8_t i = 0; i < len_1; ++i)
		{
			if (prefix[i] != rstr[i])
			{
				return false;
			}
		}
		return true;
	}
}

}
}
