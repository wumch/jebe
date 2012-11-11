
#pragma once

#include "predef.hpp"
#include <boost/dynamic_bitset.hpp>
#include <zmq.hpp>
#include "filter.hpp"
#include "recorder.hpp"

namespace jebe {
namespace idf {

class Calculater
{
public:
	typedef std::vector<docnum_t> DFList;

private:
	static const int mainid;
	int id;

	const Filter* const filter;

	zmq::context_t& context;
	zmq::socket_t sock;

	char response_data[1];
	zmq::message_t response;

	Recorder recorder;

	static DFList dflist;

	boost::dynamic_bitset<uint32_t> collector_finished;
	boost::dynamic_bitset<uint32_t> calculater_finished;

public:
	Calculater(zmq::context_t& context, int id_);

	~Calculater();

	static void init();

	void run();

	void stop();

protected:
	void calculate();

	CS_FORCE_INLINE void prepare();

	void work();

private:
	void listen();

	CS_FORCE_INLINE bool process(zmq::socket_t& sock, zmq::message_t& message);

	CS_FORCE_INLINE void attachDoc(const char* doc, size_t len);

	bool handleCollected(uint8_t collector_id);

	bool handleCalculated(uint8_t calculater_id);

	bool isMain();

	bool notifyMain();

	void reply(zmq::socket_t& sock);

	void ready();

	void filte();

	void calcu();

	void dump();

	void finish();
};

} /* namespace idf */
} /* namespace jebe */
