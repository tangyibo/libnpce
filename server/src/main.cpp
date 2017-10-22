// Defines the entry point for the Web Service application.
#include <sys/wait.h>
#include <unistd.h>       /* For pause() */
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include "HttpService.h"
#include "boost/program_options.hpp"
#include "strutils.hpp"
#include "logging.hpp"

#define EXECUTABLE "httpd_npce"
#define SERVER_VERSION "v1.0.1"
#define SERVER_COPYRIGHT "copyright @ tang"
#define DESCRIPTION "The HTTP server frontend for NPCE."

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::parse_command_line;

static int exit_flag=0;

static void signal_handler(int sig_num)
{
	exit_flag = sig_num;
}

int main(int argc, char *argv[])
{
	std::string usage = EXECUTABLE " in NPCE "SERVER_VERSION" - "SERVER_COPYRIGHT"\n";
	usage += DESCRIPTION "\n\n";
	usage += "usage: ./" EXECUTABLE " <options>\n\n";
	usage += "options";

	options_description optparser = options_description(usage);
	optparser.add_options()
	("port", value<int>(), "The port number [default=12345].")
	("threads", value<int>(), "The number of threads [default=1].")
	("log-level", value<int>(), "The log level:\n"
     		"- 0: TRACE level\n"
     		"- 1: DEBUG level\n"
     		"- 2: INFO level [default]\n")
	("help,h", "Show help information");

	if (argc == 1)
	{
		std::cerr << optparser << std::endl;
		return 1;
	}

	variables_map vm;
	store(parse_command_line(argc, argv, optparser), vm);

	if (vm.count("help"))
	{
		std::cerr << optparser << std::endl;
		return 0;
	}

	int port = 12345;
	if (vm.count("port"))
	{
		port = vm["port"].as<int>();
		if (port < 80 || port > 65535)
		{
			ERROR_LOG("port number %d is not in legal range (80 .. 65535)");
			return 1;
		}
	}

	int threads = 1;
	if (vm.count("threads"))
	{
		threads = vm["threads"].as<int>();
		if (threads < 0)
		{
			WARNING_LOG("number of threads should not less than 0, reset to 1.");
			threads = 1;
		}
	}

	int log_level = LTP_LOG_INFO;
	if (vm.count("log-level"))
	{
		log_level = vm["log-level"].as<int>();
		if (log_level == 0)
		{
			utils::Logger<void>::get_logger()->set_lvl(LTP_LOG_TRACE);
		}
		else if (log_level == 1)
		{
			utils::Logger<void>::get_logger()->set_lvl(LTP_LOG_DEBUG);
		}
	}

	std::string port_str = utils::to_str(port);
	std::string threads_str = utils::to_str(threads);

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	struct mg_context *ctx;
	const char *options[] ={ "listening_ports", port_str.c_str(), "num_threads", threads_str.c_str(),NULL };
	struct mg_callbacks callbacks;

	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = CHttpService::Service;

	if ((ctx = mg_start(&callbacks, NULL, options)) == NULL)
	{
		ERROR_LOG("Cannot initialize Mongoose context");
		INFO_LOG("please check your network configuration.");
		exit(EXIT_FAILURE);
	}

	// getchar();
	while (exit_flag == 0)
	{
		sleep(100000);
	}

	mg_stop(ctx);

	return 0;
}
