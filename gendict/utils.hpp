
#pragma once

namespace jebe {
namespace cws {

typedef uint32_t atimes_t;
typedef wchar_t	CharType;
BOOST_STATIC_ASSERT(sizeof(CharType) == 4);
typedef std::wstring String;

template<uint8_t plen> class MapHashBits { public: enum { bits = 13 }; };
template<> class MapHashBits<1> { public: enum { bits = 12 }; };
template<> class MapHashBits<2> { public: enum { bits = 24 }; };		// for 3.4GB, should 29
template<> class MapHashBits<3> { public: enum { bits = 24 }; };
template<> class MapHashBits<4> { public: enum { bits = 22 }; };
template<> class MapHashBits<5> { public: enum { bits = 20 }; };
template<> class MapHashBits<6> { public: enum { bits = 18 }; };
template<> class MapHashBits<7> { public: enum { bits = 15 }; };

template<uint8_t plen> class PadHashBits { public: enum { bits = 12 }; };
template<> class PadHashBits<1> { public: enum { bits = MapHashBits<1>::bits }; };
template<> class PadHashBits<2> { public: enum { bits = 22 }; };
template<> class PadHashBits<3> { public: enum { bits = 20 }; };
template<> class PadHashBits<4> { public: enum { bits = 18 }; };
template<> class PadHashBits<5> { public: enum { bits = 16 }; };
template<> class PadHashBits<6> { public: enum { bits = 14 }; };

template<uint8_t len_1, uint8_t len_2>
class PhraseMatch
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[len_1], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= len_1);
		for (uint8_t i = 0; i < len_1; ++i)
		{
			if (prefix[i] != rstr[i])
			{
				return false;
			}
		}
		return true;
	}
};

template<uint8_t len_2>
class PhraseMatch<1, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[1], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 1);
		return prefix[0] == rstr[0];
	}
};

template<uint8_t len_2>
class PhraseMatch<2, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[2], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 2);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1];
	}
};

template<uint8_t len_2>
class PhraseMatch<3, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[3], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 3);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2];
	}
};

template<uint8_t len_2>
class PhraseMatch<4, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[4], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 4);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3];
	}
};

template<uint8_t len_2>
class PhraseMatch<5, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[5], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 5);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4];
	}
};

template<uint8_t len_2>
class PhraseMatch<6, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[6], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 6);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4]
			 && prefix[5] == rstr[5];
	}
};

template<uint8_t len_2>
class PhraseMatch<7, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[7], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 7);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4]
			 && prefix[5] == rstr[5]
			 && prefix[6] == rstr[6];
	}
};

template<uint8_t len_2>
class PhraseMatch<8, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[8], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 8);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4]
			 && prefix[5] == rstr[5]
			 && prefix[6] == rstr[6]
			 && prefix[7] == rstr[7];
	}
};

template<uint8_t len_2>
class PhraseMatch<9, len_2>
{
public:
	static CS_FORCE_INLINE
	bool match(const CharType prefix[9], const CharType rstr[len_2])
	{
		BOOST_STATIC_ASSERT(len_2 >= 9);
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4]
			 && prefix[5] == rstr[5]
			 && prefix[6] == rstr[6]
			 && prefix[7] == rstr[7]
			 && prefix[8] == rstr[8];
	}
};

}
}
