
#include "collector.hpp"
#ifdef __linux
#	include <unistd.h>
#endif
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <zmq.hpp>
#include <msgpack.hpp>
#include <glog/logging.h>
#include "config.hpp"
#include "aside.hpp"
#include "mongoinput.hpp"

namespace jebe {
namespace idf {

Collector::Collector(zmq::context_t& context_)
	: context(context_),
	  recv_buf_area(new char[Aside::config->receive_buffer_size]),
	  recv_buf(recv_buf_area, Aside::config->receive_buffer_size, NULL, NULL),
	  send_buf_area(new char[Aside::config->send_buffer_size]),
	  send_buf(send_buf_area, Aside::config->send_buffer_size, NULL, NULL),
	  chunk_size(Aside::config->chunk_size), chunk_num(Aside::config->chunk_num)
{
	if (Aside::config->send_buffer_size < 16)
	{
		CS_DIE("send-buffer-size must not less than 16, current: " << Aside::config->send_buffer_size);
	}
}

// actually, optimized for mongodb or other "block devices" such as disk.
void Collector::run()
{
	zmq::socket_t sock(context, ZMQ_REQ);

	{
		usleep(10000);
		sock.setsockopt(ZMQ_SNDBUF, &Aside::config->send_buffer_size, sizeof(Aside::config->send_buffer_size));
		sock.setsockopt(ZMQ_RCVBUF, &Aside::config->receive_buffer_size, sizeof(Aside::config->receive_buffer_size));
		for (uint i = 0; i < Aside::config->calculater_num; ++i)
		{
			sock.connect((Aside::config->internal + "-" + boost::lexical_cast<std::string>(i)).c_str());
			LOG_IF(INFO, Aside::config->loglevel > 0) << "connect collector => calculater<" << i << ">";
		}
	}

	// since mongo is sloooower than idf-calcualter, it's better to perform memory-copy for making sending faster.
	LOG_IF(INFO, Aside::config->loglevel > 0) << "start inputing";
	std::auto_ptr<BaseInput> input = create_input();
	input->start();
	const char* doc;
	while ((doc = input->next()) &&
		((Aside::totalDocNum != 0) && (Aside::curDocNum < Aside::totalDocNum)))
	{
		process(sock, doc, get_chunk());
	}
	LOG_IF(INFO, Aside::config->loglevel > 0) << "finished inputing";
	input->stop();
}

void Collector::process(zmq::socket_t& sock, const char* doc, uint32_t chunk_turn)
{
	size_t len = std::min<size_t>(chunk_size, strlen(doc));
	memcpy(chunks[chunk_turn], doc, len);
//	void (*p) (void*, void*) = boost::bind(&Collector::recycle_chunk_, this);
	zmq::message_t message(chunks[chunk_turn], len, &Collector::recycle_chunk_, this);
	sock.send(message, ZMQ_NOBLOCK);
}

std::auto_ptr<BaseInput> Collector::create_input() const
{
	return std::auto_ptr<BaseInput>(dynamic_cast<BaseInput*>(new MongoInput));
}

void Collector::recycle_chunk_(void* chunk, void* turn)
{
//	recycle_chunk(static_cast<char*>(chunk), static_cast<char*>(turn));
}

void Collector::recycle_chunk(char* chunk, char* turn)
{
	chunks_mask[turn - chunks_mask] = 0;
}

uint32_t Collector::get_chunk()
{
	int turn = acquire_chunk();
	while (turn == -1)
	{
		usleep(10);
		turn = acquire_chunk();
	}
	return turn;
}

int Collector::acquire_chunk()
{
	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		if (chunks_mask[i] == 0)
		{
			chunks_mask[i] = 1;
			return i;
		}
	}
	return -1;		// failed
}

void Collector::prepare()
{
	chunks_mask = new char[chunk_num];
	chunks = reinterpret_cast<char**>(std::malloc(sizeof(char*) * chunk_num));
	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		chunks[i] = reinterpret_cast<char*>(std::malloc(chunk_size));
	}
}

Collector::~Collector()
{
	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		std::free(chunks[i]);
		chunks[i] = NULL;
	}
	std::free(chunks);
	chunks = NULL;
	delete chunks_mask;
	chunks_mask = NULL;
}

} /* namespace qdb */
} /* namespace jebe */
