
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
namespace cluster {
namespace kmeans {

void Config::init(int argc, char* argv[])
{
	argv_first = argv[0];
	boost::program_options::options_description cmdDesc("allowed options");
	cmdDesc.add_options()
			("help,h", "show this help and exit.")
			("config,c", boost::program_options::value<std::string>()->default_value("etc/cluster.conf"),
				"config file, defaults to etc/cluster.conf.")
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
	std::string default_listen(std::string("tcp://") + staging::getLanIP() + ":10025");
	boost::filesystem::path default_logfile(std::string("/var/log/") + program_name + ".log");
	desc.add_options()
		("listen", boost::program_options::value<typeof(listen)>()->default_value(default_listen))
		("internal", boost::program_options::value<typeof(listen)>()->default_value("inproc://cluster"),
				"internal communicate address, defaults to `inproc://cluster`")
		("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value(default_pidfile),
				(std::string("pid file, defaults to ") + default_pidfile.string()).c_str())
		("centers-output-file", boost::program_options::value<typeof(centers_outputfile)>()->default_value("etc/centers.dat"))
		("cluster-vectors-output-file", boost::program_options::value<typeof(cls_vecs_outputfile)>()->default_value("etc/cluster-vectors.txt"))
		("output-delimiter", boost::program_options::value<typeof(output_delimiter)>()->default_value("\t"))
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

		("mongo-server", boost::program_options::value<typeof(mongo_server)>()->default_value("localhost"))
		("mongo-collection", boost::program_options::value<typeof(mongo_collection)>()->default_value(""))
		("mongo-field", boost::program_options::value<typeof(mongo_collection)>()->default_value(""))
		("mongo-auto-reconnect", boost::program_options::value<typeof(mongo_auto_reconnect)>()->default_value(true))
		("mongo-max-documents", boost::program_options::value<typeof(mongo_max_doc)>()->default_value(0))

		("chunk-size", boost::program_options::value<typeof(chunk_size)>()->default_value(128))
		("chunk-num", boost::program_options::value<typeof(chunk_num)>()->default_value(10))

		("reserve-features-count", boost::program_options::value<typeof(reserve_fnum)>()->default_value(200))

		("supposed-lowest-k", boost::program_options::value<typeof(supposed_lowest_k)>()->default_value(500))
		("supposed-levels", boost::program_options::value<typeof(supposed_levels)>()->default_value(4))
		("supposed-k-before-decompose", boost::program_options::value<typeof(supposed_k_before_decompose)>()->default_value(0.05))
		("top-level-min-clusters", boost::program_options::value<typeof(top_level_min_clses)>()->default_value(20))

		("min-members-by-average", boost::program_options::value<typeof(min_members_by_avg)>()->default_value(0.05))
		("max-members-by-average", boost::program_options::value<typeof(max_members_by_avg)>()->default_value(20))
		("max-decompose", boost::program_options::value<typeof(max_decompose)>()->default_value(0.05))
		("max-separate", boost::program_options::value<typeof(max_separate)>()->default_value(0.1))

		("center-min-features-rate", boost::program_options::value<typeof(center_min_features_rate)>()->default_value(0.15))
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
	centers_outputfile = options["centers-output-file"].as<typeof(centers_outputfile)>();
	cls_vecs_outputfile = options["cluster-vectors-output-file"].as<typeof(cls_vecs_outputfile)>();
	output_delimiter = options["output-delimiter"].as<typeof(output_delimiter)>();
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

	mongo_server = options["mongo-server"].as<typeof(mongo_server)>();
	mongo_collection = options["mongo-collection"].as<typeof(mongo_collection)>();
	mongo_field = options["mongo-field"].as<typeof(mongo_field)>();
	mongo_auto_reconnect = options["mongo-auto-reconnect"].as<typeof(mongo_auto_reconnect)>();

	chunk_size = options["chunk-size"].as<typeof(chunk_size)>() << 10;
	chunk_num = options["chunk-num"].as<typeof(chunk_num)>();
	msg_max_size = options["message-max-size"].as<typeof(msg_max_size)>() << 10;
	if (!mongo_max_doc)		// not passed in via command line, or what passed in is 0.
	{
		mongo_max_doc = options["mongo-max-documents"].as<typeof(mongo_max_doc)>();
	}

	reserve_fnum = options["reserve-features-count"].as<typeof(reserve_fnum)>();
	supposed_lowest_k = options["supposed-lowest-k"].as<typeof(supposed_lowest_k)>();
	supposed_levels = options["supposed-levels"].as<typeof(supposed_levels)>();
	supposed_k_before_decompose = options["supposed-k-before-decompose"].as<typeof(supposed_k_before_decompose)>();
	top_level_min_clses = options["top-level-min-clusters"].as<typeof(top_level_min_clses)>();

	min_members_by_avg = options["min-members-by-average"].as<typeof(min_members_by_avg)>();
	max_decompose = options["max-decompose"].as<typeof(max_decompose)>();
	max_members_by_avg = options["max-members-by-average"].as<typeof(max_members_by_avg)>();
	max_separate = options["max-separate"].as<typeof(max_separate)>();

	center_min_features_rate = options["center-min-features-rate"].as<typeof(center_min_features_rate)>();

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
		_JEBE_OUT_CONFIG_PROPERTY(centers_outputfile)
		_JEBE_OUT_CONFIG_PROPERTY(cls_vecs_outputfile)
		_JEBE_OUT_CONFIG_PROPERTY(io_threads)
		_JEBE_OUT_CONFIG_PROPERTY(stack_size)
		_JEBE_OUT_CONFIG_PROPERTY(reuse_address)
		_JEBE_OUT_CONFIG_PROPERTY(receive_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(send_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(timeout)
		_JEBE_OUT_CONFIG_PROPERTY(tcp_nodelay)
		_JEBE_OUT_CONFIG_PROPERTY(msg_max_size)
		_JEBE_OUT_CONFIG_PROPERTY(max_connections)

		_JEBE_OUT_CONFIG_PROPERTY(calculater_num)
		_JEBE_OUT_CONFIG_PROPERTY(collector_num)
	);
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */

#undef _JEBE_OUT_CONFIG_PROPERTY
