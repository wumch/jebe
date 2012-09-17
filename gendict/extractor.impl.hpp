
#pragma once

#include "predef.hpp"
#include <boost/preprocessor.hpp>
#include "misc.hpp"
#include "phrase.hpp"

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
//#include BOOST_PP_LOCAL_ITERATE()

class HFHash
{
public:
	template<uint8_t n> CS_FORCE_INLINE static uint32_t hfhash(const Phrase<n>& p)
	{
		unsigned int mask = 0;
		char* res = reinterpret_cast<char*>(&mask);
		const char* data = reinterpret_cast<const char*>(p.str);
		for (uint i = 0; i < (n); ++i)
		{
			res[i & 3] ^= p.str[i];
		}
		return mask;
	}
};

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<1>(const Phrase<1>& p)
{
	return static_cast<uint16_t>(p.str[0]);
}

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<2>(const Phrase<2>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8);
}

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<3>(const Phrase<3>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<4>(const Phrase<4>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]);
}

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<5>(const Phrase<5>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<6>(const Phrase<6>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<7>(const Phrase<7>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16)  ^ (static_cast<uint16_t>(p.str[6]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t hfhash<8>(const Phrase<8>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16)  ^ (static_cast<uint16_t>(p.str[6]) << 8)  ^ (static_cast<uint16_t>(p.str[7]) << 16);
}

template<> CS_FORCE_INLINE
uint32_t HFHash::hfhash<9>(const Phrase<9>& p)
{
	return static_cast<uint16_t>(p.str[0]) ^ (static_cast<uint16_t>(p.str[1]) << 8) ^ (static_cast<uint16_t>(p.str[2]) << 16) ^ static_cast<uint16_t>(p.str[3]) ^ (static_cast<uint16_t>(p.str[4]) << 8) ^ (static_cast<uint16_t>(p.str[5]) << 16)  ^ static_cast<uint16_t>(p.str[6]) ^ (static_cast<uint16_t>(p.str[7]) << 8)  ^ (static_cast<uint16_t>(p.str[8]) << 16);
}

}
}
