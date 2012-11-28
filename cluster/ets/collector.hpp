
#pragma once

#include "predef.hpp"
#include <vector>
#include <memory>
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include <zmq.hpp>
#include <msgpack.hpp>
#include "autoincr.hpp"
#include "input.hpp"
#include "input_document.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace ets {

class Collector
{
public:
	Collector(zmq::context_t& context, uint8_t id_);

	void run();

	void stop();

	~Collector();

private:
	void work();

	void process(const InDocument* doc);

	size_t convert(const InDocument* doc, char* chunk, size_t chunk_size_);

	size_t pack_out_doc(const OutDocument& outdoc, char* zone);

	std::auto_ptr<BaseInput> create_input() const;

	void recycle_in_chunk(char* chunk, uint32_t turn);

	static void recycle_in_chunk_(void* chunk, void* owner);

	uint32_t get_in_chunk();

	int acquire_in_chunk();

	void recycle_out_chunk(char* chunk, uint32_t turn);

	static void recycle_out_chunk_(void* chunk, void* owner);

	uint32_t get_out_chunk();

	int acquire_out_chunk();

	void prepare();
	void send_config();

	void notify();	// notify finish

private:
	zmq::socket_t& get_sock();

	uint8_t id;		// collector-id

	typedef staging::AutoIncr<staging::AutoIncrType<Document>, vid_t, 1> VecIdGen;
	VecIdGen* vidgen;

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

	char** in_chunks;
	boost::dynamic_bitset<uint64_t> in_chunks_mask;

	char** out_chunks;
	boost::dynamic_bitset<uint64_t> out_chunks_mask;

	msgpack::sbuffer packer_buffer;

	FILE* docs;	// processed docs.

};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
