
#pragma once

#include "predef.hpp"
#include <boost/dynamic_bitset.hpp>
#include <zmq.hpp>
#include "aside.hpp"
#include "config.hpp"
#include "filter.hpp"
#include "recorder.hpp"

namespace jebe {
namespace idf {

class Calculater
{
public:
	typedef std::vector<docnum_t> DFList;

private:
	int id;

	Ftree* ftree;

	Filter* filter;

	zmq::context_t& context;

	Recorder recorder;

	DFList dflist;

public:
	Calculater(zmq::context_t& context, int id_);

	~Calculater();

	void run();

	void stop();

protected:
	void calculate();

	CS_FORCE_INLINE void prepare();

	void work();

private:
	CS_FORCE_INLINE void process(zmq::socket_t& sock, zmq::message_t& message);

	CS_FORCE_INLINE void attachDoc(const char* doc, size_t len);

	void ready();

	void filte();

	void calcu();

	void dump();

	void finish();
};

} /* namespace idf */
} /* namespace jebe */
