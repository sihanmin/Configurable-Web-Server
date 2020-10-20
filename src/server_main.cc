//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <time.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "session.h"
#include "server.h"
#include "config_parser.h"
#include "dispatcher.h"

using boost::asio::ip::tcp;

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void init_logging()
{
  logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");

  const char *format_str = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%";
  const char *log_file_name = "sample.log";

  logging::add_file_log(
      keywords::file_name = "file_%10N.log",
      keywords::rotation_size = 10 * 1024 * 1024,
      keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0,0,0),
      keywords::format = format_str,
      keywords::auto_flush = true);
  logging::add_console_log(
      std::cout, keywords::format = format_str);

  logging::add_common_attributes();
}

void handler(const boost::system::error_code &error, int signal_number)
{
  std::cerr << "Termination: SIGINT" << std::endl;
  BOOST_LOG_TRIVIAL(fatal) << "Termination: SIGINT";
  exit(1);
}

int main(int argc, char *argv[])
{

  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <path-to-config-file>" << std::endl;
      return 1;
    }

    init_logging();

    NginxConfigParser parser;
    NginxConfig config;
    if (!parser.Parse(argv[1], &config))
    {
      std::cerr << "Error: invalid config file" << std::endl;
      BOOST_LOG_TRIVIAL(fatal) << "Config file parsing failed: Invalid config file";
      return 1;
    }
    int port_num = config.GetPortNumber();
    if (port_num == -1)
    {
      std::cerr << "Error: invalid port number specified in config file" << std::endl;
      BOOST_LOG_TRIVIAL(fatal) << "Config file parsing failed: Invalid port number specified in config file";

      return 1;
    }
    
    srand(time(NULL));

    dispatcher dispatcher_(config);

    BOOST_LOG_TRIVIAL(info) << "Config file parsing completed";

    BOOST_LOG_TRIVIAL(info) << "Server startup";
    
    boost::asio::io_service io_service;
    server s(io_service, port_num, &dispatcher_);

    // Construct a signal set registered for process termination.
    boost::asio::signal_set signals(io_service, SIGINT);

    // Start an asynchronous wait for one of the signals to occur.
    signals.async_wait(handler);
    
    std::size_t nthreads = 10;
    std::vector<std::unique_ptr<boost::thread>> pool;
    for (std::size_t i = 0; i != nthreads; i++) {
      std::unique_ptr<boost::thread> threadi = std::make_unique<boost::thread>(boost::bind(&boost::asio::io_service::run, &io_service));
      pool.emplace_back(std::move(threadi));
    }
    for (std::size_t i = 0; i != nthreads; i++) {
      pool[i]->join();
    }
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
