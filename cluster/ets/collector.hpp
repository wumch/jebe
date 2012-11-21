
#pragma once

#include "predef.hpp"
#include <vector>
#include <memory>
#include <boost/dynamic_bitset.hpp>
#include <zmq.hpp>
#include <msgpack.hpp>
#include "input.hpp"
#include "document.hpp"

namespace jebe {
namespace cluster {
namespace preprocess {

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

	OutDocument* convert(const InDocument* doc);

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

private:
	zmq::socket_t& get_sock();

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

	char** in_chunks;
	boost::dynamic_bitset<uint64_t> in_chunks_mask;

	char** out_chunks;
	boost::dynamic_bitset<uint64_t> out_chunks_mask;

	msgpack::sbuffer packer_buffer;

};

} /* namespace preprocess */
} /* namespace cluster */
} /* namespace jebe */
