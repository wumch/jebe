
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

Collector::Collector(zmq::context_t& context_, uint8_t id_)
	: id(id_), context(context_),
	  recv_buf_size(32), recv_buf_area(new char[recv_buf_size]),
	  recv_buf(recv_buf_area, recv_buf_size, NULL, NULL),
	  send_buf_area(new char[Aside::config->send_buffer_size]),
	  send_buf(send_buf_area, Aside::config->send_buffer_size, NULL, NULL),
	  chunk_size(Aside::config->chunk_size), chunk_num(Aside::config->chunk_num), next_sock(0)
{
	if (Aside::config->send_buffer_size < 16)
	{
		CS_DIE("send-buffer-size must not less than 16, current: " << Aside::config->send_buffer_size);
	}
}

// actually, optimized for mongodb or other "block devices" such as disk.
// NOTE: since we have to notify all of @Calculater from every @Collector, @zmq::device is impossible.
void Collector::run()
{
	prepare();
	// since mongo is sloooower than idf-calcualter, it's better to perform memory-copy for making sending faster.
	LOG_IF(INFO, Aside::config->loglevel > 0) << "collector<" << static_cast<int>(id) << "> start inputing";
	std::auto_ptr<BaseInput> input = create_input();
	input->start();
	const char* doc;
	while ((doc = input->next()) &&
		((Aside::curDocNum < Aside::totalDocNum) || (Aside::totalDocNum == 0)))
	{
		process(doc);
	}

	LOG_IF(INFO, Aside::config->loglevel > 0) << "collector<" << static_cast<int>(id) << "> finishing inputing";
	char note[2] = {static_cast<uint8_t>(collected), static_cast<uint8_t>(id)};
	for (Socks::iterator it = socks.begin(); it != socks.end(); ++it)
	{
		zmq::message_t notice(note, 2, NULL, NULL);
		(*it)->send(notice, ZMQ_NOBLOCK);
		zmq::message_t response(recv_buf_area, recv_buf_size, NULL, NULL);
		(*it)->recv(&response, 0);
	}
	input->stop();
	stop();
}

void Collector::process(const char* doc)
{
	char* const chunk = chunks[getChunk()];
	size_t len = std::min<size_t>(chunk_size - 1, strlen(doc));
	memcpy(chunk + 1, doc, len);

	zmq::socket_t& sock = getSock();
	zmq::message_t message(chunk, len, &Collector::recycle_chunk_, this);
	sock.send(message, ZMQ_NOBLOCK);

	recv_buf.rebuild(recv_buf_area, recv_buf_size, NULL, NULL);
	sock.recv(&recv_buf, 0);
}

zmq::socket_t& Collector::getSock()
{
	if (++next_sock >= socks.size())
	{
		next_sock = 0;
	}
	return *socks[next_sock];
}

std::auto_ptr<BaseInput> Collector::create_input() const
{
	return std::auto_ptr<BaseInput>(dynamic_cast<BaseInput*>(new MongoInput));
}

void Collector::recycle_chunk_(void* chunk, void* owner)
{
	Collector* self = reinterpret_cast<Collector*>(owner);
	for (uint32_t i = 0; i < self->chunk_num; ++i)
	{
		if (self->chunks[i] == chunk)
		{
			self->recycle_chunk(static_cast<char*>(chunk), i);
		}
	}
}

void Collector::recycle_chunk(char* chunk, uint32_t turn)
{
	chunks_mask[turn] = 0;
}

uint32_t Collector::getChunk()
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
	chunks = reinterpret_cast<char**>(malloc(sizeof(char*) * chunk_num));
	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		chunks[i] = reinterpret_cast<char*>(malloc(chunk_size));
		chunks[i][0] = static_cast<uint8_t>(send_doc);
	}

	socks.reserve(Aside::config->calculater_num);
	for (uint i = 0; i < Aside::config->calculater_num; ++i)
	{
		zmq::socket_t* sock = new zmq::socket_t(context, ZMQ_REQ);
		sock->setsockopt(ZMQ_SNDBUF, &Aside::config->send_buffer_size, sizeof(Aside::config->send_buffer_size));
		sock->setsockopt(ZMQ_RCVBUF, &recv_buf_size, sizeof(recv_buf_size));
		sock->connect((Aside::config->internal + "-" + boost::lexical_cast<std::string>(i)).c_str());
		socks.push_back(sock);
	}
}

void Collector::stop()
{
	for (Socks::iterator it = socks.begin(); it != socks.end(); ++it)
	{
		(*it)->close();
	}
	socks.clear();
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
