
#include "collector.hpp"
#ifdef __linux
#	include <unistd.h>
#endif
#include <iostream>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <zmq.hpp>
#include <msgpack.hpp>
#include <glog/logging.h>
#include "misc.hpp"
#include "config.hpp"
#include "aside.hpp"
#include "mongoinput.hpp"
#include "transfer.hpp"
#include "../rconfig.hpp"

namespace jebe {
namespace cluster {
namespace ets {

Collector::Collector(zmq::context_t& context_, uint8_t id_)
	: id(id_), vidgen(VecIdGen::instance()), context(context_),
	  recv_buf_size(32), recv_buf_area(new char[recv_buf_size]),
	  recv_buf(recv_buf_area, recv_buf_size, NULL, NULL),
	  send_buf_area(new char[Aside::config->send_buffer_size]),
	  send_buf(send_buf_area, Aside::config->send_buffer_size, NULL, NULL),
	  chunk_size(Aside::config->chunk_size), chunk_num(Aside::config->chunk_num), next_sock(0),
	  in_chunks_mask(chunk_num, 0), out_chunks_mask(chunk_num, 0),
	  packer_buffer(Aside::config->doc_packer_buf_size)
{
	docs = fopen(Aside::config->docid_outputfile.string().c_str(), "w");
	in_chunks_mask.flip();
	out_chunks_mask.flip();
	if (Aside::config->send_buffer_size < 16)
	{
		CS_DIE("send-buffer-size must not less than 16, current: " << Aside::config->send_buffer_size);
	}
}

// actually, optimized for mongodb or other "block devices" such as disk.
// NOTE: since we have to notify all of @Transfer from every @Collector, @zmq::device is impossible.
void Collector::run()
{
	prepare();
	send_config();

	// since mongo is sloooower than transfer, it's better to perform memory-copy for making sending faster.
	LOG_IF(INFO, Aside::config->loglevel > 0) << "collector<" << static_cast<int>(id) << "> start inputing";
	std::auto_ptr<BaseInput> input = create_input();
	char* chunk;
	input->start();
	while (input->more() && ((Aside::curDocNum < Aside::totalDocNum) || (Aside::totalDocNum == 0)))
	{
		uint32_t chunk_idx = get_in_chunk();
		chunk = in_chunks[chunk_idx];
		if (!input->next(chunk))
		{
			recycle_in_chunk(chunk, chunk_idx);
			break;
		}
		process(reinterpret_cast<InDocument*>(chunk));
		recycle_in_chunk(chunk, chunk_idx);
	}
	input->stop();

	LOG_IF(INFO, Aside::config->loglevel > 0) << "finished input";
	notify();
	stop();
}

void Collector::process(const InDocument* indoc)
{
	CS_RETURN_IF(!indoc);

	char* chunk = out_chunks[get_out_chunk()];
	size_t msg_size = convert(indoc, chunk + 1, chunk_size - 1) + 1;
	if (msg_size == 1)
	{
		recycle_out_chunk_(chunk, this);
		return;
	}
	__sync_add_and_fetch(&Aside::curDocNum, 1);
	zmq::socket_t& sock = get_sock();
	zmq::message_t message(chunk, msg_size, &Collector::recycle_out_chunk_, this);
	sock.send(message, ZMQ_NOBLOCK);

	recv_buf.rebuild(recv_buf_area, recv_buf_size, NULL, NULL);
	sock.recv(&recv_buf, 0);
}

size_t Collector::convert(const InDocument* indoc, char* chunk, size_t chunk_size_)
{
	Document doc(vidgen->gen());
	Aside::transfer->trans(*indoc, doc.flist);
	CS_RETURN_IF(doc.flist.empty(), 0);
	fprintf(docs, "%d\t", doc.id);
	static_cast<void>(fwrite(indoc->_id, indoc->_id_size, 1, docs));
	static_cast<void>(fwrite("\t", CS_CONST_STRLEN("\t"), 1, docs));
	static_cast<void>(fwrite(indoc->url, indoc->url_size, 1, docs));
	static_cast<void>(fwrite("\n", CS_CONST_STRLEN("\n"), 1, docs));
	packer_buffer.clear();
	msgpack::packer<msgpack::sbuffer> packer(packer_buffer);
	packer.pack(doc);
	CS_RETURN_IF(chunk_size_ > chunk_size, 0);
	memcpy(chunk, packer_buffer.data(), packer_buffer.size());
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
	std::auto_ptr<BaseInput> input(dynamic_cast<BaseInput*>(new MongoInput));
	// do some basic checking?
	return input;
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
	}

	out_chunks = reinterpret_cast<char**>(malloc(sizeof(char*) * chunk_num));
	for (uint32_t i = 0; i < chunk_num; ++i)
	{
		out_chunks[i] = reinterpret_cast<char*>(malloc(chunk_size));
		out_chunks[i][0] = static_cast<uint8_t>(send_doc);
	}

	socks.reserve(Aside::config->calculaters.size());
	for (CalculaterList::const_iterator it = Aside::config->calculaters.begin(); it != Aside::config->calculaters.end(); ++it)
	{
		std::auto_ptr<zmq::socket_t> sock(new zmq::socket_t(context, ZMQ_REQ));
		sock->setsockopt(ZMQ_SNDBUF, &Aside::config->send_buffer_size, sizeof(Aside::config->send_buffer_size));
		sock->setsockopt(ZMQ_RCVBUF, &recv_buf_size, sizeof(recv_buf_size));
		sock->connect(it->c_str());
		socks.push_back(sock.release());
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

void Collector::notify()
{
	LOG_IF(INFO, Aside::config->loglevel > 0) << "collector<" << static_cast<int>(id) << "> finishing inputing";
	char note[2] = {static_cast<uint8_t>(thats_all), static_cast<uint8_t>(id)};
	for (Socks::iterator it = socks.begin(); it != socks.end(); ++it)
	{
		zmq::message_t notice(note, 2, NULL, NULL);
		(*it)->send(notice, ZMQ_NOBLOCK);
		zmq::message_t response(recv_buf_area, recv_buf_size, NULL, NULL);
		(*it)->recv(&response, 0);
	}
}

void Collector::send_config()
{
	Aside::rconfig->reserve_fnum = Aside::config->reserve_fnum;
	Aside::rconfig->supposed_lowest_k = Aside::config->supposed_lowest_k;
	Aside::rconfig->total_fnum = Aside::wordsNum();
	Aside::rconfig->total_vnum = Aside::config->mongo_max_doc;
	if (Aside::rconfig->total_vnum == 0)
	{
		CS_DIE("kid, <RConfig>.total_vnum=0 is bad, we can *not* handle all of documents.");
	}

	msgpack::sbuffer packer_buffer(sizeof(RConfig) << 2);
	msgpack::packer<msgpack::sbuffer> packer(packer_buffer);
	packer.pack(*Aside::rconfig);

	size_t msgsize = packer_buffer.size() + sizeof(char);
	char* msgbody(new char[msgsize]);
	msgbody[0] = static_cast<char>(tell_config);
	memcpy(msgbody + sizeof(char), packer_buffer.data(), packer_buffer.size());

	zmq::message_t holder(msgbody, msgsize, NULL, NULL);
	for (Socks::iterator it = socks.begin(); it != socks.end(); ++it)
	{
		zmq::message_t config_message(holder.size());
		config_message.copy(&holder);
		(*it)->send(config_message, ZMQ_NOBLOCK);
		zmq::message_t response(recv_buf_area, recv_buf_size, NULL, NULL);
		(*it)->recv(&response, 0);
	}
}

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
