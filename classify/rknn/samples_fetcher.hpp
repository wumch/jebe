
#pragma once

#include "predef.hpp"
#include "vector.hpp"

namespace jebe {
namespace classify {
namespace rknn {

// samples and ropts fetcher.
class SamplesFetcher
{
private:
	SamplePool& samples;

public:
	SamplesFetcher()
		: samples(Aside::samples)
	{}
};


// fetch samples from file.
class SFFile
{

};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
