
#pragma once

#include "predef.hpp"
#include <iostream>
#include <msgpack.hpp>
#include "singleton.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

#define _JEBE_OUT_RCONFIG_PROPERTY(property)		<< #property << ":\t\t" << property << std::endl

#define __JEBE_OUT_RCONFIG_CONTENT	   				\
	"remote-config:" << std::endl					\
	_JEBE_OUT_RCONFIG_PROPERTY(total_fnum)			\
	_JEBE_OUT_RCONFIG_PROPERTY(total_vnum)			\
	_JEBE_OUT_RCONFIG_PROPERTY(supposed_lowest_k)	\
	_JEBE_OUT_RCONFIG_PROPERTY(reserve_fnum)

// remote-config, be used to keep consistency among all of workers.
class RConfig
{
public:
	template<typename T> friend T* staging::getInstance();
public:
	CS_FORCE_INLINE static RConfig* getInstance()
	{
		return staging::getInstance<RConfig>();
	}

	fnum_t total_fnum;
	vnum_t total_vnum;

	clsnum_t supposed_lowest_k;

	fnum_t reserve_fnum;

public:
	RConfig()
		: total_fnum(0), total_vnum(0),
		  supposed_lowest_k(0), reserve_fnum(0)
	{}

	void output()
	{
		CS_SAY(__JEBE_OUT_RCONFIG_CONTENT);
		LOG(INFO) << __JEBE_OUT_RCONFIG_CONTENT;
	}

	MSGPACK_DEFINE(total_fnum, total_vnum, supposed_lowest_k, reserve_fnum);
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
