
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
namespace cluster {
namespace preprocess {

Collector::Collector(zmq::context_t& context_, uint8_t id_)
	: id(id_), context(context_),
	  recv_buf_size(32), recv_buf_area(new char[recv_buf_size]),
	  recv_buf(recv_buf_area, recv_buf_size, NULL, NULL),
	  send_buf_area(new char[Aside::config->send_buffer_size]),
	  send_buf(send_buf_area, Aside::config->send_buffer_size, NULL, NULL),
	  chunk_size(Aside::config->chunk_size), chunk_num(Aside::config->chunk_num), next_sock(0),
	  in_chunks_mask(chunk_num, true), out_chunks_mask(chunk_num, true),
	  packer_buffer(0)
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
	char* chunk;
	while ((Aside::curDocNum < Aside::totalDocNum) || (Aside::totalDocNum == 0))
	{
		chunk = in_chunks[get_in_chunk()];
		input->next(chunk);
		process(reinterpret_cast<InDocument*>(chunk));
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

OutDocument* Collector::convert(const InDocument* doc)
{
	return new OutDocument;		// TODO
}

void Collector::process(const InDocument* indoc)
{
	CS_RETURN_IF(!*indoc);

	OutDocument* outdoc = convert(indoc);
	char* chunk = out_chunks[get_out_chunk()];
	size_t msg_size = pack_out_doc(*outdoc, chunk);

	zmq::socket_t& sock = get_sock();
	zmq::message_t message(chunk, msg_size, &Collector::recycle_out_chunk_, this);
	sock.send(message, ZMQ_NOBLOCK);

	recv_buf.rebuild(recv_buf_area, recv_buf_size, NULL, NULL);
	sock.recv(&recv_buf, 0);
}

size_t Collector::pack_out_doc(const OutDocument& outdoc, char* zone)
{
	packer_buffer.clear();
//	packer_buffer.base::data = zone;
//	packer_buffer.base::alloc = chunk_size;
	msgpack::packer<msgpack::sbuffer> packer(packer_buffer);
	packer.pack(outdoc);
	return packer_buffer.size();
}

zmq::socket_t& Collector::get_sock()
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

void Collector::recycle_in_chunk_(void* chunk, void* owner)
{
	Collector* self = reinterpret_cast<Collector*>(owner);
	for (uint32_t i = 0; i < self->chunk_num; ++i)
	{
		if (self->in_chunks[i] == chunk)
		{
			self->recycle_in_chunk(static_cast<char*>(chunk), i);
		}
	}
}

void Collector::recycle_in_chunk(char* chunk, uint32_t turn)
{
	in_chunks_mask.set(turn, true);
}

uint32_t Collector::get_in_chunk()
{
	int turn = acquire_in_chunk();
	while (turn == -1)
	{
		usleep(10);
		turn = acquire_in_chunk();
	}
	return turn;
}

int Collector::acquire_in_chunk()
{
	size_t idle = in_chunks_mask.find_first();
	if (CS_BLIKELY(idle != in_chunks_mask.npos))
	{
		in_chunks_mask.set(idle, false);
		return idle;
	}
	else
	{
		return -1;
	}
}

// out-chunk
void Collector::recycle_out_chunk_(void* chunk, void* owner)
{
	Collector* self = reinterpret_cast<Collector*>(owner);
	for (uint32_t i = 0; i < self->chunk_num; ++i)
	{
		if (self->out_chunks[i] == chunk)
		{
			self->recycle_out_chunk(static_cast<char*>(chunk), i);
		}
	}
}

void Collector::recycle_out_chunk(char* chunk, uint32_t turn)
{
	out_chunks_mask.set(turn, true);
}

uint32_t Collector::get_out_chunk()
{
	int turn = acquire_out_chunk();
	while (turn == -1)
	{
		usleep(10);
		turn = acquire_out_chunk();
	}
	return turn;
}

int Collector::acquire_out_chunk()
{
	size_t idle = out_chunks_mask.find_first();
	if (CS_BLIKELY(idle != out_chunks_mask.npos))
	{
		out_chunks_mask.set(idle, false);
		return idle;
	}
	else
	{
		return -1;
	}
}

void Collector::prepare()
{
	in_chunks = reinterpret_cast<char**>(malloc(sizeof(char*) * chunk_num));
	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		in_chunks[i] = reinterpret_cast<char*>(malloc(chunk_size));
		in_chunks[i][0] = static_cast<uint8_t>(send_doc);
	}

	out_chunks = reinterpret_cast<char**>(malloc(sizeof(char*) * chunk_num));
	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		out_chunks[i] = reinterpret_cast<char*>(malloc(chunk_size));
		out_chunks[i][0] = static_cast<uint8_t>(send_doc);
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
		std::free(in_chunks[i]);
		in_chunks[i] = NULL;
	}
	std::free(in_chunks);
	in_chunks = NULL;

	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		std::free(out_chunks[i]);
		out_chunks[i] = NULL;
	}
	std::free(out_chunks);
	out_chunks = NULL;
}

} /* namespace preprocess */
} /* namespace cluster */
} /* namespace jebe */
