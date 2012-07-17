
#include "config.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <unistd.h>
//#include <LuaPlus/LuaPlus.h>

namespace jebe {
namespace cws {

void Config::init(int argc, const char* const argv[])
{
	boost::program_options::options_description cmdDesc("allowed options");
	cmdDesc.add_options()
			("help", "show this help and exit.")
			("config", bo::value<std::string>()->default_value("etc/tokenizer.conf"), "config file, defaults to etc/tokenizer.conf.")
	;

	boost::filesystem::path program_path = argv[0];
	program_name = program_path.filename().string();

	boost::program_options::variables_map cmdOptions;
	try
	{
		bo::store(bo::parse_command_line(argc, argv, cmdDesc, bo::command_line_style::unix_style), cmdOptions);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n" << cmdDesc << std::endl;
	}
	bo::notify(cmdOptions);

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
	desc.add_options()
			("host", bo::value<typeof(host)>()->default_value("0.0.0.0"),
					"host, defaults to 0.0.0.0")
			("port", bo::value<typeof(port)>()->default_value(10087),
					"port, defaults to 10087")
			("pid-file", bo::value<typeof(pidfile)>()->default_value("/var/run/tokenizer.pid"),
					"pid file, defaults to /var/run/tokenizer.pid")
			("patten-file", bo::value<typeof(patten_file)>()->default_value("etc/words.txt"),
					"pattens file, defaults to etc/words.txt")
			("reuse-address", bo::value<typeof(reuse_address)>()->default_value(true),
					"whether reuse-address on startup or not")
			("receive-buffer-size", bo::value<typeof(receive_buffer_size)>()->default_value((4 << 10)),
					"siz eof receive buffer (in bytes)")
			("send-buffer-size", bo::value<typeof(send_buffer_size)>()->default_value((4 << 10)),
					"size of send buffer (in bytes)")
			("timeout", bo::value<typeof(timeout)>()->default_value(3000),
					"timeout for incoming conections (in ms), 0 stands for never timeout, defaults to 3000")
			("tcp-nodelay", bo::value<typeof(tcp_nodelay)>()->default_value(true),
					"enables tcp-nodelay feature or not, defaults is on")
			("worker-count", bo::value<typeof(worker_count)>()->default_value(sysconf(_SC_NPROCESSORS_CONF)))
			("header-max-size", bo::value<typeof(header_max_size)>()->default_value(1024))
			("body-max-size", bo::value<typeof(body_max_size)>()->default_value(64000))
			("max-connections", bo::value<typeof(max_connections)>()->default_value(10000))
			("max-write-times", bo::value<typeof(max_write_times)>()->default_value(100))
			("request-max-size", bo::value<typeof(request_max_size)>()->default_value(64000 + 1024))
			;
}

void Config::load(const std::string& config_file)
{
//    LuaPlus::LuaStateOwner state;
//    state->DoFile(config_file);
//
//    host.assign(state->GetGlobal("host").GetString());
//    port = state->GetGlobal("port").GetInteger();
//    pidfile.assign(state->GetGlobal("pidfile").GetString());
//    patten_file.assign(state->GetGlobal("patten_file").GetString());
//    reuse_address = state->GetGlobal("reuse_address").GetBoolean();
//    receive_buffer_size = state->GetGlobal("receive_buffer_size").GetInteger();
//    send_buffer_size = state->GetGlobal("send_buffer_size").GetInteger();
//    timeout = state->GetGlobal("timeout").GetInteger();
//    tcp_nodelay = state->GetGlobal("tcp_nodelay").GetBoolean();
	try
	{
		bo::store(bo::parse_config_file<CharType>(config_file.c_str(), desc), options);
	}
	catch (std::exception& e)
	{
		CS_DIE(L"faild on read/parse config-file: " << config_file.c_str() << L"\n" << e.what());
	}
	bo::notify(options);

	host = options["host"].as<typeof(host)>();
	port = options["port"].as<typeof(port)>();
	pidfile = options["pid-file"].as<typeof(pidfile)>();
	patten_file = options["patten-file"].as<typeof(patten_file)>();
	reuse_address = options["reuse-address"].as<typeof(reuse_address)>();;
	receive_buffer_size = options["receive-buffer-size"].as<typeof(receive_buffer_size)>();;
	send_buffer_size = options["send-buffer-size"].as<typeof(send_buffer_size)>();;
	timeout = options["timeout"].as<typeof(timeout)>();
	tcp_nodelay = options["tcp-nodelay"].as<typeof(tcp_nodelay)>();;
	worker_count = options["worker-count"].as<typeof(worker_count)>();;
	header_max_size = options["header-max-size"].as<typeof(header_max_size)>();;
	body_max_size = options["body-max-size"].as<typeof(body_max_size)>();;
	max_connections = options["max-connections"].as<typeof(max_connections)>();;
	max_write_times = options["max-write-times"].as<typeof(max_write_times)>();;
	request_max_size = options["request-max-size"].as<typeof(request_max_size)>();

#if CS_DEBUG
	std::cout << host << std::endl
			<< port << std::endl
			<< pidfile << std::endl
			<< patten_file << std::endl
			<< reuse_address << std::endl
			<< receive_buffer_size << std::endl
			<< send_buffer_size << std::endl
			<< timeout << std::endl
			<< tcp_nodelay << std::endl
			<< worker_count << std::endl
			<< header_max_size << std::endl
			<< body_max_size << std::endl
			<< max_connections << std::endl
			<< max_write_times << std::endl
			<< request_max_size << std::endl;
#endif

//    worker_cout = state->GetGlobal("worker_count").GetInteger();
//    header_max_len = state->GetGlobal("header_max_len").GetInteger();
//    body_max_len = state->GetGlobal("body_max_len").GetInteger();
//    max_match = state->GetGlobal("max_match").GetInteger();
//    max_connections = state->GetGlobal("max_connections").GetInteger();
//    max_write_times = state->GetGlobal("max_write_times").GetInteger();
//
//    replacement = state->GetGlobal("replacement").GetString()[0];
//
//    request_max_size = header_max_size + body_max_size;
}

namespace G {
const char* config_file = NULL;
const Config* config = NULL;
}

}
}
