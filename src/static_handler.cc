// code from: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp03/http/server/request_handler.cpp 
//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include "static_handler.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.h"
#include "reply.h"

std::string urlDecode(std::string &eString);


std::shared_ptr<request_handler> static_handler::Init(const std::string& path_prefix, const NginxConfig& config) 
{
  static_handler* handler = new static_handler;
  auto statement = config.statements_.begin();
  handler->doc_root_ = (*statement)->tokens_[1];
  return std::shared_ptr<request_handler>(handler);
}

reply static_handler::handleRequest(request& request_)
{
  std::string request_path = doc_root_ + urlDecode(request_.uri_);

  // Determine the file extension.
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::ifstream is(request_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
    return response_generator::stock_reply(reply::not_found);

  // Fill out the reply to be sent to the client.
  reply reply_;
  reply_.code_ = reply::ok;
	is.seekg (0, is.end);
  int length = is.tellg();
	//std::cout << length << '\n';
  is.seekg (0, is.beg);
  char buf[length];
  //while (is.read(buf, sizeof(buf)).gcount() > 0)
    //reply_.content.append(buf, is.gcount());
	is.read(buf, length);
	reply_.body_.append(buf, length);
  reply_.headers_.insert({"Content-Length", 
    boost::lexical_cast<std::string>(reply_.body_.size())});
  reply_.headers_.insert({"Content-Type", 
    http::server::mime_types::extension_to_type(extension)});
  return reply_;
}
