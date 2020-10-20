//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
// Adapted from https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp03/http/server/request_parser.cpp
//
#include <iostream>

#include "request_parser.h"
#include "request.h"

request_parser::request_parser()
  : state_(method_start)
{
}

void request_parser::reset()
{
  state_ = method_start;
}

//boost::tribool request_parser::consume(request& req, char input)
boost::tribool request_parser::consume(std::string& req_method, std::string& req_uri, std::vector<header>& req_headers, int& req_http_version_major, int& req_http_version_minor, char input)
{
  switch (state_)
  {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      //std::cout << "method start" << std::endl;
      return false;
    }
    else
    {
      state_ = method;
      req_method.push_back(input);
      return boost::indeterminate;
    }
  case method:
    if (input == ' ')
    {
      state_ = uri;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      //std::cout << "method" << std::endl;
      return false;
    }
    else
    {
      req_method.push_back(input);
      return boost::indeterminate;
    }
  case uri:
    if (input == ' ')
    {
      state_ = http_version_h;
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      //std::cout << "uri" << std::endl;
      return false;
    }
    else
    {
      req_uri.push_back(input);
      return boost::indeterminate;
    }
  case http_version_h:
    if (input == 'H')
    {
      state_ = http_version_t_1;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version h" << std::endl;
      return false;
    }
  case http_version_t_1:
    if (input == 'T')
    {
      state_ = http_version_t_2;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version t 1" << std::endl;
      return false;
    }
  case http_version_t_2:
    if (input == 'T')
    {
      state_ = http_version_p;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version t 2" << std::endl;
      return false;
    }
  case http_version_p:
    if (input == 'P')
    {
      state_ = http_version_slash;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version p" << std::endl;
      return false;
    }
  case http_version_slash:
    if (input == '/')
    {
      req_http_version_major = 0;
      req_http_version_minor = 0;
      state_ = http_version_major_start;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version slash" << std::endl;
      return false;
    }
  case http_version_major_start:
    if (is_digit(input))
    {
      req_http_version_major = req_http_version_major * 10 + input - '0';
      state_ = http_version_major;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version major start" << std::endl;
      return false;
    }
  case http_version_major:
    if (input == '.')
    {
      state_ = http_version_minor_start;
      return boost::indeterminate;
    }
    else if (is_digit(input))
    {
      req_http_version_major = req_http_version_major * 10 + input - '0';
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version major" << std::endl;
      return false;
    }
  case http_version_minor_start:
    if (is_digit(input))
    {
      //std::cout << "http version minor start is " << input << std::endl;
      req_http_version_minor = req_http_version_minor * 10 + input - '0';
      state_ = http_version_minor;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "http version minor start" << std::endl;
      return false;
    }
  case http_version_minor:
    if (input == '\r')
    {
      state_ = expecting_newline_1;
      return boost::indeterminate;
    }
    else if (is_digit(input))
    {
      req_http_version_minor = req_http_version_minor * 10 + input - '0';
      return boost::indeterminate;
    }
    else
    {
      //std::cout << input;
      //std::cout << "http version minor" << std::endl;
      return false;
    }
  case expecting_newline_1:
    if (input == '\n')
    {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "expecting newline 1" << std::endl;
      return false;
    }
  case header_line_start:
    if (input == '\r')
    {
      state_ = expecting_newline_3;
      return boost::indeterminate;
    }
    else if (!req_headers.empty() && (input == ' ' || input == '\t'))
    {
      state_ = header_lws;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      //std::cout << "header line start" << std::endl;
      return false;
    }
    else
    {
      req_headers.push_back(header());
      req_headers.back().name.push_back(input);
      state_ = header_name;
      return boost::indeterminate;
    }
  case header_lws:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (input == ' ' || input == '\t')
    {
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      //std::cout << "header lws" << std::endl;
      return false;
    }
    else
    {
      state_ = header_value;
      req_headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case header_name:
    if (input == ':')
    {
      state_ = space_before_header_value;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      //std::cout << "header name" << std::endl;
      return false;
    }
    else
    {
      req_headers.back().name.push_back(input);
      return boost::indeterminate;
    }
  case space_before_header_value:
    if (input == ' ')
    {
      state_ = header_value;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "space before header value" << std::endl;
      return false;
    }
  case header_value:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      //std::cout << "header value" << std::endl;
      return false;
    }
    else
    {
      req_headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case expecting_newline_2:
    if (input == '\n')
    {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else
    {
      //std::cout << "expecting newline 2" << std::endl;
      return false;
    }
  case expecting_newline_3:
    return (input == '\n');
  default:
      //std::cout << "default" << std::endl;
    return false;
  }
}

bool request_parser::is_char(int c)
{
  return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(int c)
{
  return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c)
{
  switch (c)
  {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c)
{
  return c >= '0' && c <= '9';
}

