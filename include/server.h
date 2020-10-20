#ifndef server_h
#define server_h

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "reply.h"
#include "request_parser.h"

class ServerTestF;
class server
{
public:
  server(boost::asio::io_service& io_service, short port, dispatcher* dispatcher_);
  void handle_accept(session* new_session,
      const boost::system::error_code& error, bool test);
  void start_accept();

  friend ServerTestF;
  
private:

  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  dispatcher* dispatcher_;
};


#endif
