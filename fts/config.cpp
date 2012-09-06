
#ifndef __GNUC__
#	error "before resolving config options, please make sure your compiler has `typeof(var)` supported"
#endif

#include "config.hpp"
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>
extern "C" {
#include <sched.h>
}
#include "sys.hpp"
#include "net.hpp"

#define _JEBE_OUT_CONFIG_PROPERTY(property)		<< #property << ":\t\t" << property << std::endl

namespace jebe {
namespace fts {

void Config::init(int argc, char* argv[])
{
	boost::program_options::options_description cmdDesc("allowed options");
	cmdDesc.add_options()
			("help", "show this help and exit.")
			("config", boost::program_options::value<std::string>()->default_value("etc/fts.conf"),
					"config file, defaults to etc/tokenizer.conf.")
	;

	boost::filesystem::path program_path = argv[0];
#if BOOST_VERSION > 104200
	program_name = program_path.filename().string();
#else
	program_name = program_path.filename();
#endif

	boost::program_options::variables_map cmdOptions;
	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdDesc, boost::program_options::command_line_style::unix_style), cmdOptions);
	}
	catch (std::exception& e)
	{
		CS_DIE(e.what() << "\n" << cmdDesc);
	}
	boost::program_options::notify(cmdOptions);

	if (cmdOptions.count("help"))
	{
		std::cout << cmdDesc << std::endl;
	}
	else if (cmdOptions.count("config"))
	{
		initDesc();
		load(cmdOptions["config"].as<std::string>());
	}
}

void Config::initDesc()
{
	std::string default_listen(std::string("tcp://") + staging::getNatIP() + ":10050");
	desc.add_options()
		("listen", boost::program_options::value<typeof(listen)>()->default_value(default_listen))
		("internal", boost::program_options::value<typeof(listen)>()->default_value("inproc://fts"),
				"internal communicate address, defaults to `inproc://fts`")
		("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value("/var/run/tokenizer.pid"),
				"pid file, defaults to /var/run/tokenizer.pid")
		("db-path", boost::program_options::value<typeof(dbpath)>()->default_value("etc/ftsdb"),
				"leveldb path, defaults to etc/ftsdb")
		("reuse-address", boost::program_options::value<typeof(reuse_address)>()->default_value(true),
				"whether reuse-address on startup or not, default is on")
		("receive-buffer-size", boost::program_options::value<typeof(receive_buffer_size)>()->default_value((4 << 10)),
				"siz eof receive buffer (in bytes), default is 4KB")
		("send-buffer-size", boost::program_options::value<typeof(send_buffer_size)>()->default_value((4 << 10)),
				"size of send buffer (in bytes), default is 4KB")
		("timeout", boost::program_options::value<typeof(timeout)>()->default_value(3000),
				"timeout for incoming conections (in ms), 0 stands for never timeout, defaults to 3000")
		("tcp-nodelay", boost::program_options::value<typeof(tcp_nodelay)>()->default_value(true),
				"enables tcp-nodelay feature or not, default is on")
		("memlock", boost::program_options::value<typeof(memlock)>()->default_value(false))
		("stack-size", boost::program_options::value<typeof(stack_size)>()->default_value(staging::getRlimitCur(RLIMIT_STACK)))
		("cpuaffinity", boost::program_options::value<std::string>()->default_value(""))
		("worker-count", boost::program_options::value<typeof(worker_count)>()->default_value(staging::getCpuNum() - 1))
		("io-threads", boost::program_options::value<typeof(io_threads)>()->default_value(1))
		("message-max-size", boost::program_options::value<typeof(msg_max_size)>()->default_value(100 << 20))
		("max-connections", boost::program_options::value<typeof(max_connections)>()->default_value(10000))
		;
}

void Config::load(const std::string& config_file)
{
	try
	{
		boost::program_options::store(boost::program_options::parse_config_file<char>(config_file.c_str(), desc), options);
	}
	catch (std::exception& e)
	{
		CS_DIE("faild on read/parse config-file: " << config_file.c_str() << "\n" << e.what());
	}
	boost::program_options::notify(options);
	listen = options["listen"].as<typeof(listen)>();
	internal = options["internal"].as<typeof(internal)>();
	pidfile = options["pid-file"].as<typeof(pidfile)>();
	dbpath = options["db-path"].as<typeof(dbpath)>();
	reuse_address = options["reuse-address"].as<typeof(reuse_address)>();
	receive_buffer_size = options["receive-buffer-size"].as<typeof(receive_buffer_size)>();
	send_buffer_size = options["send-buffer-size"].as<typeof(send_buffer_size)>();
	timeout = options["timeout"].as<typeof(timeout)>();
	tcp_nodelay = options["tcp-nodelay"].as<typeof(tcp_nodelay)>();
	worker_count = options["worker-count"].as<typeof(worker_count)>();
	io_threads = options["io-threads"].as<typeof(io_threads)>();
	msg_max_size = options["message-max-size"].as<typeof(msg_max_size)>();
	max_connections = options["max-connections"].as<typeof(max_connections)>();

	memlock = options["memlock"].as<typeof(memlock)>();
	if (memlock)
	{
#if CS_DEBUG
		int res = mlockall(MCL_CURRENT | MCL_FUTURE);
		assert(!res);
#else
		mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
	}

	// `cpu-affinity` no longer useable since the threads-model changed.
	std::string cpumask = options["cpuaffinity"].as<std::string>();

	typedef boost::char_separator<char> Separator;
	typedef boost::tokenizer<Separator> Tokenizer;
	Separator sep("[],");
	Tokenizer tokens(cpumask, sep);

	if (cpumask.size())
	{
		uint cpunum = sysconf(_SC_NPROCESSORS_CONF);
		uint bits = cpunum * worker_count;
		if (cpumask.size() < bits)
		{
			cpumask.append(bits - cpumask.size(), '0');
		}
		cpuaffinity = boost::dynamic_bitset<>(cpumask);
	}

	CS_SAY("configs in <" << config_file << ">:" << std::endl
		_JEBE_OUT_CONFIG_PROPERTY(listen)
		_JEBE_OUT_CONFIG_PROPERTY(internal)
		_JEBE_OUT_CONFIG_PROPERTY(pidfile)
		_JEBE_OUT_CONFIG_PROPERTY(worker_count)
		_JEBE_OUT_CONFIG_PROPERTY(io_threads)
		_JEBE_OUT_CONFIG_PROPERTY(stack_size)
		_JEBE_OUT_CONFIG_PROPERTY(reuse_address)
		_JEBE_OUT_CONFIG_PROPERTY(receive_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(send_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(timeout)
		_JEBE_OUT_CONFIG_PROPERTY(tcp_nodelay)
		_JEBE_OUT_CONFIG_PROPERTY(msg_max_size)
		_JEBE_OUT_CONFIG_PROPERTY(max_connections)
	);
}

}
}

#undef _JEBE_OUT_CONFIG_PROPERTY
