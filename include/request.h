//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
// Adapted from https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp03/http/server/request.hpp
//

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>

/// A request received from a client.
/*
struct request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
  std::string content;
};
*/

struct request
{
  enum MethodEnum
  {
    GET,
    PUT,
    POST
  } method_;
  std::string uri_;
  std::string version_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

#endif // HTTP_REQUEST_HPP
