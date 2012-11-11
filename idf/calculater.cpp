
#include "calculater.hpp"
#include <fstream>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <zmq.hpp>
#include <glog/logging.h>
#include "aside.hpp"
#include "filter.hpp"

namespace jebe {
namespace idf {

const int Calculater::mainid = 0;
Calculater::DFList Calculater::dflist;

Calculater::Calculater(zmq::context_t& context_, int id_)
	: id(id_), filter(Aside::filter), context(context_),
	  sock(context, ZMQ_REP), response(response_data, 1, NULL, NULL),
	  collector_finished(Aside::config->collector_num, 0),
	  calculater_finished(Aside::config->calculater_num, 0)
{
	listen();
}

void Calculater::run()
{
	prepare();
	work();
	finish();
}

void Calculater::work()
{
	// since mongo is sloooower than idf-calcualter, it's better to perform memory-copy for making sending faster.
	LOG_IF(INFO, Aside::config->loglevel > 0) << "start inputing";
	char* const msgbuf = new char[Aside::config->msg_max_size];
	zmq::message_t message(msgbuf, Aside::config->msg_max_size, NULL, NULL);
	while (sock.recv(&message, 0))
	{
		if (CS_BUNLIKELY(!process(sock, message)))
		{
			break;
		}
		message.rebuild(msgbuf, Aside::config->msg_max_size, NULL, NULL);
	}
	if (isMain())
	{
		calculate();
	}
}

void Calculater::calculate()
{
	ready();
	filte();
	calcu();
}

void Calculater::ready()
{

}

void Calculater::filte()
{

}

void Calculater::finish()
{
	stop();
}

void Calculater::calcu()
{
	uint64_t total_df = 0;
	for (DFList::const_iterator it = dflist.begin(); it != dflist.end(); ++it)
	{
		total_df += *it;
	}
	LOG_IF(INFO, Aside::config->loglevel > 0) << "total document-frequency: " << total_df;

	if (CS_BUNLIKELY(total_df == 0))
	{
		LOG_IF(INFO, Aside::config->loglevel > 0) << "total document-frequency == 0, so that no need of dump";
		return;
	}

	std::ofstream ofile(Aside::config->outputfile.string().c_str(), std::ios_base::trunc);
	for (wordnum_t i = 0, end = Aside::wordsNum(); i < end; ++i)
	{
		if (dflist[i] == 0)
		{
			ofile << Aside::wordList[i] << '\t' << 0 << CS_LINESEP;
		}
		else
		{
			ofile << Aside::wordList[i] << '\t' << (CS_BUNLIKELY(dflist[i] == Aside::curDocNum) ? 0 : (-std::log10(dflist[i] / Aside::curDocNum))) << CS_LINESEP;
		}
	}
	ofile.close();
	LOG_IF(INFO, Aside::config->loglevel > 0) << "dumped to " << Aside::config->outputfile.string();
}

void Calculater::prepare()
{
	response_data[0] = 1;		// anyway response "1".
}

void Calculater::init()
{
	dflist.resize(Aside::wordsNum(), 0);

	if (Aside::config->record_on_cache)
	{
		for (DFList::const_iterator it = dflist.begin(); it != dflist.end(); ++it)
		{
			CS_PREFETCH(&*it, 1, 3);
		}
	}
}

// true: continue with receiving, false: stop receiveing.
bool Calculater::process(zmq::socket_t& sock, zmq::message_t& message)
{
	reply(sock);
	CS_RETURN_IF(message.size() < 1, true);
	Action act = static_cast<Action>(*static_cast<uint8_t*>(message.data()));
	if (CS_BLIKELY(act == send_doc))
	{
		attachDoc(static_cast<char*>(message.data()) + 1, message.size() - 1);
		return true;
	}
	else if (act == collected)
	{
		return handleCollected(*(static_cast<uint8_t*>(message.data()) + 1));
	}
	else if (act == calculated)
	{
		return handleCalculated(*(static_cast<uint8_t*>(message.data()) + 1));
	}
	else
	{
		return false;
	}
}

void Calculater::attachDoc(const char* doc, size_t len)
{
	CS_RETURN_IF(len < 1);
	recorder.reset();
	__sync_add_and_fetch(&Aside::curDocNum, 1);
	filter->find(reinterpret_cast<const byte_t*>(doc), len, recorder);
	const Recorder::NodeList& nodes = recorder.recordedNodes();
	for (Recorder::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		__sync_fetch_and_add(&dflist[(*it)->pattenid], 1);
	}
}

void Calculater::listen()
{
	static const int64_t send_buf_size = 32;
	sock.setsockopt(ZMQ_SNDBUF, &send_buf_size, sizeof(send_buf_size));
	sock.setsockopt(ZMQ_RCVBUF, &Aside::config->receive_buffer_size, sizeof(Aside::config->receive_buffer_size));
	sock.bind((Aside::config->internal + "-" + boost::lexical_cast<std::string>(id)).c_str());
}

bool Calculater::handleCollected(uint8_t collector_id)
{
	collector_finished.set(collector_id, true);
	LOG_IF(INFO, Aside::config->loglevel > 0) << "collector<" << static_cast<int>(collector_id) << "> finished. (" << collector_finished.count() << "/" << Aside::config->collector_num << ")";
	if (collector_finished.count() == Aside::config->collector_num)
	{
		return notifyMain();
	}
	else
	{
		return true;
	}
}
bool Calculater::notifyMain()
{
	if (isMain())
	{
		return handleCalculated(id);
	}
	else
	{
		zmq::socket_t sock(context, ZMQ_REQ);
		sock.connect((Aside::config->internal + "-" + boost::lexical_cast<std::string>(mainid)).c_str());
		char note[2] = {static_cast<uint8_t>(calculated), static_cast<uint8_t>(id)};
		zmq::message_t notice(note, 2, NULL, NULL);
		sock.send(notice, ZMQ_NOBLOCK);
		zmq::message_t response;
		sock.recv(&response, 0);
		sock.close();
		return false;
	}
}

bool Calculater::handleCalculated(uint8_t calculater_id)
{
	CS_RETURN_IF(!isMain(), false);
	calculater_finished.set(calculater_id, true);
	LOG_IF(INFO, Aside::config->loglevel > 0) << "calculater<" << static_cast<int>(calculater_id) << "> finished. (" << calculater_finished.count() << "/" << Aside::config->calculater_num << ")";
	if (calculater_finished.count() == Aside::config->calculater_num)
	{
		LOG_IF(INFO, Aside::config->loglevel > 0) << "all were calculated";
		sock.close();
		return false;
	}
	else
	{
		return true;
	}
}

bool Calculater::isMain()
{
	return id == mainid;
}

void Calculater::reply(zmq::socket_t& sock)
{
	response.rebuild(response_data, 1, NULL, NULL);
	sock.send(response, ZMQ_NOBLOCK);
}

void Calculater::stop()
{
}

Calculater::~Calculater()
{}

} /* namespace idf */
} /* namespace jebe */
