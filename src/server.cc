#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <set>
#include "server.h"

server::server(boost::asio::io_service& io_service, short port, dispatcher* dispatcher_)
  : io_service_(io_service), dispatcher_(dispatcher_), 
    acceptor_(io_service, 
      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
  start_accept();
}

void server::start_accept()
{
  session* new_session = new session(io_service_, dispatcher_);
  acceptor_.async_accept(new_session->socket(),
      boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error, false));
}

void server::handle_accept(session* new_session,
          const boost::system::error_code& error, bool test)
{
  if (!error)
  {
    new_session->start();
  }
  else
  {
    delete new_session;
  }

  if (!test)
  {
    start_accept();
  }
  
}
