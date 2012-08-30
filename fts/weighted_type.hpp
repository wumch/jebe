
#pragma once

#include <msgpack.hpp>

namespace jebe {
namespace fts {

typedef long double weight_t;

template<typename ObjectType, typename WeightType = weight_t>
class WeightedType
{
public:
	ObjectType object;
	WeightType weight;

	ObjectType& first;
	WeightType& second;

public:
	WeightedType()
		: first(object), second(weight)
	{

	}

	WeightedType(const ObjectType& object_, WeightType weight_)
		: object(object_), weight(weight_),
		  first(object), second(weight)
	{

	}

	MSGPACK_DEFINE(object, weight);
};

}
}
