
#pragma once

#include "predef.hpp"
#include <memory>
#include <zmq.hpp>
#include "input.hpp"

namespace jebe {
namespace idf {

class Collector
{
public:
	Collector(zmq::context_t& context);

	void run();

	void stop();

	~Collector();

private:
	void work();

	void process(zmq::socket_t& sock, const char* doc, uint32_t chunk_turn);

	std::auto_ptr<BaseInput> create_input() const;

	void recycle_chunk(char* chunk, char* turn);

	static void recycle_chunk_(void* chunk, void* owner);

	uint32_t get_chunk();

	int acquire_chunk();

	void prepare();

	zmq::context_t& context;

	char* recv_buf_area;
	zmq::message_t recv_buf;

	char* send_buf_area;
	zmq::message_t send_buf;

	uint32_t chunk_size;
	uint32_t chunk_num;
	char** chunks;
	char* chunks_mask;
};

} /* namespace qdb */
} /* namespace jebe */
