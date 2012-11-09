
#include "calculater.hpp"
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <zmq.hpp>
#include <glog/logging.h>
#include "aside.hpp"
#include "filter.hpp"

namespace jebe {
namespace idf {

Calculater::Calculater(zmq::context_t& context_, int id_)
	: id(id_), context(context_)
{}

void Calculater::run()
{
	prepare();
	work();
	finish();
}

void Calculater::work()
{
	zmq::socket_t sock(context, ZMQ_REP);

	{
		sock.setsockopt(ZMQ_SNDBUF, &Aside::config->send_buffer_size, sizeof(Aside::config->send_buffer_size));
		sock.setsockopt(ZMQ_RCVBUF, &Aside::config->receive_buffer_size, sizeof(Aside::config->receive_buffer_size));
		sock.bind((Aside::config->internal + "-" + boost::lexical_cast<std::string>(id)).c_str());
		LOG_IF(INFO, Aside::config->loglevel > 0) << "calculater<" << id << "> listen on " << (Aside::config->internal + ":" + boost::lexical_cast<std::string>(id));
	}

	// since mongo is sloooower than idf-calcualter, it's better to perform memory-copy for making sending faster.
	LOG_IF(INFO, Aside::config->loglevel > 0) << "start inputing";
	char* const msgbuf = new char[Aside::config->msg_max_size];
	zmq::message_t message(msgbuf, Aside::config->msg_max_size, NULL, NULL);
	while (sock.recv(&message, 0))
	{
		message.rebuild(msgbuf, Aside::config->msg_max_size, NULL, NULL);
		process(sock, message);
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
	LOG_IF(INFO, Aside::config->loglevel > 0) << "done";
}

void Calculater::calcu()
{
	uint64_t total_df = 0;
	for (DFList::const_iterator it = dflist.begin(); it != dflist.end(); ++it)
	{
		total_df += *it;
	}
	LOG_IF(INFO, Aside::config->loglevel > 0) << "total document-frequency: " << total_df;

	std::ofstream ofile(Aside::config->outputfile.string().c_str(), std::ios_base::trunc);
	for (wordnum_t i = 0, end = Aside::wordsNum(); i < end; ++i)
	{
		if (dflist[i] == 0)
		{
			ofile << Aside::wordList[i] << '\t' << 0 << CS_LINESEP;
		}
		else
		{
			ofile << Aside::wordList[i] << '\t' << (-std::log10(dflist[i] / total_df)) << CS_LINESEP;
		}
	}
	ofile.close();
}

void Calculater::prepare()
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

void Calculater::process(zmq::socket_t& sock, zmq::message_t& message)
{
	Action act = static_cast<Action>(*static_cast<char*>(message.data()));
	if (CS_BLIKELY(act == send_doc))
	{
		attachDoc(reinterpret_cast<char*>(message.data()), message.size());
	}
	else if (act == thats_all)
	{
		LOG_IF(INFO, Aside::config->loglevel > 0) << "finished inputing";
		calculate();
	}
}

void Calculater::attachDoc(const char* doc, size_t len)
{
	recorder.reset();
	filter->find(reinterpret_cast<const byte_t*>(doc), len, recorder);
	const Recorder::NodeList& nodes = recorder.recordedNodes();
	for (Recorder::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		__sync_fetch_and_add(&dflist[(*it)->pattenid], 1);
	}
}

Calculater::~Calculater()
{}

} /* namespace idf */
} /* namespace jebe */
