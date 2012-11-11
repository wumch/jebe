
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
extern "C" {
#include <sched.h>
}
#include "sys.hpp"
#include "net.hpp"

#define _JEBE_OUT_CONFIG_PROPERTY(property)		<< #property << ":\t\t" << property << std::endl

namespace jebe {
namespace idf {

void Config::init(int argc, char* argv[])
{
	argv_first = argv[0];
	boost::program_options::options_description cmdDesc("allowed options");
	cmdDesc.add_options()
			("help,h", "show this help and exit.")
			("config,c", boost::program_options::value<std::string>()->default_value("etc/idf.conf"),
				"config file, defaults to etc/idf.conf.")
			("mongo-max-documents,m", boost::program_options::value<typeof(mongo_max_doc)>()->default_value(0),
				"max-documents should be processed.")
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
	catch (const std::exception& e)
	{
		CS_DIE(e.what() << "\n" << cmdDesc);
	}
	boost::program_options::notify(cmdOptions);

	if (cmdOptions.count("mongo-max-documents"))
	{
		mongo_max_doc = cmdOptions["mongo-max-documents"].as<typeof(mongo_max_doc)>();
	}
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
	boost::filesystem::path default_pidfile(std::string("/var/run/") + program_name + ".pid");
	std::string default_listen(std::string("tcp://") + staging::getLanIP() + ":10023");
	boost::filesystem::path default_logfile(std::string("/var/log/") + program_name + ".log");
	desc.add_options()
		("listen", boost::program_options::value<typeof(listen)>()->default_value(default_listen))
		("internal", boost::program_options::value<typeof(listen)>()->default_value("inproc://idf"),
				"internal communicate address, defaults to `inproc://idf`")
		("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value(default_pidfile),
				(std::string("pid file, defaults to ") + default_pidfile.string()).c_str())
		("patten-file", boost::program_options::value<typeof(pattenfile)>()->default_value("etc/patten.txt"),
				"pid file, defaults to etc/patten.txt")
		("output-file", boost::program_options::value<typeof(outputfile)>()->default_value("etc/patten.idf.txt"))
		("log-file", boost::program_options::value<typeof(logfile)>()->default_value(default_logfile))
		("log-level", boost::program_options::value<typeof(loglevel)>()->default_value(0))
		("reuse-address", boost::program_options::value<typeof(reuse_address)>()->default_value(true),
				"whether reuse-address on startup or not, default is on")
		("receive-buffer-size", boost::program_options::value<typeof(receive_buffer_size)>()->default_value((64)),
				"siz eof receive buffer (in bytes), default is 64KB")
		("send-buffer-size", boost::program_options::value<typeof(send_buffer_size)>()->default_value((4)),
				"size of send buffer (in bytes), default is 4KB")
		("timeout", boost::program_options::value<typeof(timeout)>()->default_value(3000),
				"timeout for incoming conections (in ms), 0 stands for never timeout, defaults to 3000")
		("tcp-nodelay", boost::program_options::value<typeof(tcp_nodelay)>()->default_value(true),
				"enables tcp-nodelay feature or not, default is on")
		("memlock", boost::program_options::value<typeof(memlock)>()->default_value(false))
		("record-on-cache", boost::program_options::value<typeof(record_on_cache)>()->default_value(false))
		("stack-size", boost::program_options::value<typeof(stack_size)>()->default_value(staging::getRlimitCur(RLIMIT_STACK)))
		("cpuaffinity", boost::program_options::value<std::string>()->default_value(""))

		("io-threads", boost::program_options::value<typeof(io_threads)>()->default_value(1))
		("collector-num", boost::program_options::value<typeof(collector_num)>()->default_value(1))
		("calculater-num", boost::program_options::value<typeof(calculater_num)>()->default_value(1))

		("message-max-size", boost::program_options::value<typeof(msg_max_size)>()->default_value(100 << 20))
		("max-connections", boost::program_options::value<typeof(max_connections)>()->default_value(10000))

		("max-open-files", boost::program_options::value<typeof(max_open_files)>()->default_value(staging::getRlimitCur(RLIMIT_NOFILE)))
		("block-size", boost::program_options::value<typeof(block_size)>()->default_value(32))
		("block-cache", boost::program_options::value<typeof(block_cache)>()->default_value(256))
		("write-buffer-size", boost::program_options::value<typeof(write_buffer_size)>()->default_value(256))
		("max-retrieve-elements", boost::program_options::value<typeof(max_retrieve_elements)>()->default_value(50))

		("mongo-server", boost::program_options::value<typeof(mongo_server)>()->default_value("localhost"))
		("mongo-collection", boost::program_options::value<typeof(mongo_collection)>()->default_value(""))
		("mongo-field", boost::program_options::value<typeof(mongo_collection)>()->default_value(""))
		("mongo-auto-reconnect", boost::program_options::value<typeof(mongo_auto_reconnect)>()->default_value(true))
		("mongo-max-documents", boost::program_options::value<typeof(mongo_max_doc)>()->default_value(0))

		("chunk-size", boost::program_options::value<typeof(chunk_size)>()->default_value(128))
		("chunk-num", boost::program_options::value<typeof(chunk_num)>()->default_value(10))
		;
}

void Config::load(const std::string& config_file)
{
	try
	{
		boost::program_options::store(boost::program_options::parse_config_file<char>(config_file.c_str(), desc), options);
	}
	catch (const std::exception& e)
	{
		CS_DIE("faild on read/parse config-file: " << config_file.c_str() << "\n" << e.what());
	}
	boost::program_options::notify(options);
	listen = options["listen"].as<typeof(listen)>();
	internal = options["internal"].as<typeof(internal)>();
	pidfile = options["pid-file"].as<typeof(pidfile)>();
	pattenfile = options["patten-file"].as<typeof(pattenfile)>();
	outputfile = options["output-file"].as<typeof(outputfile)>();
	logfile = options["log-file"].as<typeof(logfile)>();
	loglevel = options["log-level"].as<typeof(loglevel)>();
	reuse_address = options["reuse-address"].as<typeof(reuse_address)>();
	receive_buffer_size = options["receive-buffer-size"].as<typeof(receive_buffer_size)>() << 10;
	send_buffer_size = options["send-buffer-size"].as<typeof(send_buffer_size)>() << 10;
	timeout = options["timeout"].as<typeof(timeout)>();
	tcp_nodelay = options["tcp-nodelay"].as<typeof(tcp_nodelay)>();

	io_threads = options["io-threads"].as<typeof(io_threads)>();
	calculater_num = options["calculater-num"].as<typeof(calculater_num)>();
	collector_num = options["collector-num"].as<typeof(collector_num)>();

	max_connections = options["max-connections"].as<typeof(max_connections)>();

	max_open_files = options["max-open-files"].as<typeof(max_open_files)>();
	block_size = options["block-size"].as<typeof(block_size)>() << 10;
	block_cache = options["block-cache"].as<typeof(block_cache)>() << 20;
	write_buffer_size = options["write-buffer-size"].as<typeof(write_buffer_size)>() << 20;
	max_retrieve_elements = options["max-retrieve-elements"].as<typeof(max_retrieve_elements)>();

	mongo_server = options["mongo-server"].as<typeof(mongo_server)>();
	mongo_collection = options["mongo-collection"].as<typeof(mongo_collection)>();
	mongo_field = options["mongo-field"].as<typeof(mongo_field)>();
	mongo_auto_reconnect = options["mongo-auto-reconnect"].as<typeof(mongo_auto_reconnect)>();
	mongo_max_doc = options["mongo-max-documents"].as<typeof(mongo_max_doc)>();

	chunk_size = options["chunk-size"].as<typeof(chunk_size)>() << 10;
	chunk_num = options["chunk-num"].as<typeof(chunk_num)>();
	msg_max_size = options["message-max-size"].as<typeof(msg_max_size)>() << 10;
	if (!mongo_max_doc)		// not passed in via command line, or what passed in is 0.
	{
		mongo_max_doc = options["mongo-max-documents"].as<typeof(mongo_max_doc)>();
	}

	memlock = options["memlock"].as<typeof(memlock)>();
	record_on_cache = options["record-on-cache"].as<typeof(record_on_cache)>();
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
		uint bits = cpunum * (collector_num + calculater_num);
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
		_JEBE_OUT_CONFIG_PROPERTY(outputfile)
		_JEBE_OUT_CONFIG_PROPERTY(io_threads)
		_JEBE_OUT_CONFIG_PROPERTY(stack_size)
		_JEBE_OUT_CONFIG_PROPERTY(reuse_address)
		_JEBE_OUT_CONFIG_PROPERTY(receive_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(send_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(timeout)
		_JEBE_OUT_CONFIG_PROPERTY(tcp_nodelay)
		_JEBE_OUT_CONFIG_PROPERTY(msg_max_size)
		_JEBE_OUT_CONFIG_PROPERTY(max_connections)

		_JEBE_OUT_CONFIG_PROPERTY(max_open_files)
		_JEBE_OUT_CONFIG_PROPERTY(block_size)
		_JEBE_OUT_CONFIG_PROPERTY(block_cache)
		_JEBE_OUT_CONFIG_PROPERTY(write_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(max_retrieve_elements)

		_JEBE_OUT_CONFIG_PROPERTY(calculater_num)
		_JEBE_OUT_CONFIG_PROPERTY(collector_num)
	);
}

}
}

#undef _JEBE_OUT_CONFIG_PROPERTY
