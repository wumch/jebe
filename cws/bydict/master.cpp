
#include <vector>
#include <boost/thread.hpp>
#include "master.hpp"
#include "config.hpp"

namespace jebe {
namespace cws {

typedef std::vector<boost::shared_ptr<boost::thread> > ThreadList;

Master::Master(const std::size_t worker_count)
    : next_worker_index(0), workers(worker_count), sess(),
    buf_sready(G::session_ready, sizeof(G::session_ready))
{
    for (WorkerPool::iterator iter = workers.begin(); iter != workers.end(); ++iter)
    {
        iter->reset(new Worker());
    }
}

void Master::notify_worker(const SessId sid)
{
    if (!(++next_worker_index < workers.size()))
    {
        next_worker_index = 0;
    }
    G::sids_map[next_worker_index].push_back(sid);
    pthread_kill(G::worker_id_list[next_worker_index], SIGUSR1);
}

void Master::run()
{
    ThreadList threads;
    for (WorkerPool::iterator iter = workers.begin(); iter != workers.end(); ++iter)
    {
        iter->reset(new Worker());
        boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&Worker::run, *iter)));
        threads.push_back(thread);
    }

    filter = new Filter(Config::getInstance()->patten_file, Config::getInstance()->replacement);
    
    listen();
    for (ThreadList::iterator iter = threads.begin(); iter != threads.end(); ++iter)
    {
        (*iter)->join();
    }
}

void Master::stop()
{
    for (WorkerPool::iterator iter = workers.begin(); iter != workers.end(); ++iter)
    {
        (*iter)->stop();
    }
}

void Master::listen()
{
	namespace BA = boost::asio;
    acptor = new BA::ip::tcp::acceptor(get_io());
    Config* config = Config::getInstance();
    
    BA::ip::tcp::resolver resolver(acptor->get_io_service());
    BA::ip::tcp::resolver::query query(config->host,
    boost::lexical_cast<std::string>(config->port));
    BA::ip::tcp::endpoint ep = *resolver.resolve(query);
    acptor->open(ep.protocol());

    acptor->set_option(BA::ip::tcp::acceptor::reuse_address(config->reuse_address));
    acptor->set_option(BA::ip::tcp::acceptor::send_buffer_size(config->send_buffer_size));
    acptor->set_option(BA::ip::tcp::acceptor::receive_buffer_size(config->receive_buffer_size));

    Session::header_max_len = config->header_max_len;
    Session::body_max_len = config->body_max_len;
    Session::max_len = Session::header_max_len + Session::body_max_len + sizeof(HTTP_SEP);
    Session::timeout = (0 < config->timeout) ? config->timeout : 1;
    Session::max_write_times = (0 < config->max_write_times) ? config->max_write_times : 10;
    
    acptor->bind(ep);
    acptor->listen();
    start_accept();
}

void Master::start_accept()
{
    sess = SessPtr(new Session(get_io()));
    acptor->async_accept(sess->getSock(),
        boost::bind(&Master::handle_accept, this,
            sess, BA::placeholders::error
        )
    );
}

BA::io_service& Master::get_io()
{
    return workers[pick_worker()]->get_io();
}

std::size_t Master::pick_worker()
{
    ++next_worker_index;
    if (next_worker_index == workers.size())
    {
        next_worker_index = 0;
    }
    return next_worker_index;
}

void Master::handle_accept(SessPtr& sess,
    const BS::error_code& err_code)
{
    if (!err_code)
    {
        sess->start();
    }
    start_accept();
}

void Master::handle_sessid(SockPtr& sock, const Session::ReadBuf& buf,
    const BS::error_code& err_code, const std::size_t bytes_transfered)
{
    SessId sid = peek_sess_id(sock, buf, bytes_transfered);
    if (err_code || sid == 0)
    {
        destroySock(sock);
    }
    else
    {
        BA::async_write(*sock, buf_sready,
            boost::bind(&Master::test_handle_write, this,
                BA::placeholders::error
            )
        );
    }
}

void Master::test_handle_write(const BS::error_code& error)
{
    
}

SessId Master::peek_sess_id(SockPtr& sock, const Session::ReadBuf& buf,
    const std::size_t bytes_transfered) const
{
    return 1;
}

}
}
