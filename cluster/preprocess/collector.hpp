
#pragma once

#include "predef.hpp"
#include <vector>
#include <memory>
#include <zmq.hpp>
#include "input.hpp"

namespace jebe {
namespace idf {

class Collector
{
public:
	Collector(zmq::context_t& context, uint8_t id_);

	void run();

	void stop();

	~Collector();

private:
	void work();

	void process(const char* doc);

	std::auto_ptr<BaseInput> create_input() const;

	void recycle_chunk(char* chunk, uint32_t turn);

	static void recycle_chunk_(void* chunk, void* owner);

	uint32_t getChunk();

	int acquire_chunk();

	void prepare();

private:
	zmq::socket_t& getSock();

	uint8_t id;

	zmq::context_t& context;
	typedef std::vector<zmq::socket_t*> Socks;
	Socks socks;

	int64_t recv_buf_size;
	char* recv_buf_area;
	zmq::message_t recv_buf;

	char* send_buf_area;
	zmq::message_t send_buf;

	uint32_t chunk_size;
	uint32_t chunk_num;
	uint32_t next_sock;
	char** chunks;
	char* chunks_mask;
};

} /* namespace qdb */
} /* namespace jebe */
