#ifndef session_h
#define session_h


#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <vector>

#include "reply.h"
#include "request.h"
#include "request_parser.h"
#include "header.h"
#include "dispatcher.h"

class SessionTestF;
class request_handler;

class session
{
public:
  session(boost::asio::io_service& io_service, 
    dispatcher* dispatcher_);

  virtual boost::asio::ip::tcp::socket& socket();

  virtual void start();

  friend SessionTestF;

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);

  void form_request(size_t& bytes_transferred, bool use_buffer);

  boost::asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  std::string buffer_;
  int buffer_length;
  request buffer_request_;
  
  /// The incoming request.
  request request_;
  std::string req_method;
  std::string req_uri;
  std::vector<header> req_headers;
  int http_version_major;
  int http_version_minor;

  /// The parser for the incoming request.
  request_parser request_parser_;

  //request_handler* handler;
  reply reply_;

  dispatcher* dispatcher_;

};

#endif
