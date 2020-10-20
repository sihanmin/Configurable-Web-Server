#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include <boost/log/trivial.hpp>

const short echo_request = 1;
const short static_request = 2;

session::session(boost::asio::io_service& io_service, 
  dispatcher* dispatcher_)
    : socket_(io_service), dispatcher_(dispatcher_) {}

boost::asio::ip::tcp::socket& session::socket()
{
  return socket_;
}

void session::start()
{
  buffer_length = 0;
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
      boost::bind(&session::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
{
  if (!error)
  {
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        req_method, req_uri, req_headers, http_version_major, 
        http_version_minor, data_, data_ + bytes_transferred);
    form_request(bytes_transferred,false);


    if (result)
    {
      if (buffer_length == 0 && request_.headers_.count("Content-Length") > 0 &&
          bytes_transferred < std::stoi(request_.headers_["Content-Length"])) {
        buffer_request_ = request_;
        buffer_.append(std::string(data_, bytes_transferred));
        buffer_length = buffer_.length();
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
         std::ofstream out("../request_buffer1.txt");
    out << buffer_;
    out.close();
        return;

      }
      else if (buffer_length != 0) {
        buffer_.append(std::string(data_, bytes_transferred));
        buffer_length = buffer_.length();
        if ( buffer_length < std::stoi(buffer_request_.headers_["Content-Length"])) {
          socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
          std::ofstream out("../request_buffer2.txt");
    out << buffer_;
    out.close();
          return;
        }
        form_request(bytes_transferred,true);
      }
      reply_ = dispatcher_->call_handler(request_);
      boost::asio::async_write(socket_, response_generator::to_buffers(reply_),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
      
    }
    else if (!result)
    {
      reply_ = response_generator::stock_reply(reply::bad_request);
      boost::asio::async_write(socket_, response_generator::to_buffers(reply_),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));

      dispatcher_->log("error", "Bad request:\n" + request_.body_);
    }
    else
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }
  else
  {
    delete this;
  }
}

void session::handle_write(const boost::system::error_code& error)
{
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }
  else
  {
    delete this;
  }
}

void session::form_request(size_t& bytes_transferred, bool use_buffer)
{
  if (use_buffer) {
    request_ = buffer_request_;
    request_.body_ = buffer_;
    buffer_ = "";
    buffer_length = 0;
    std::ofstream out("../request_body.txt");
    out << request_.body_;
    out.close();
    return;
  }
  if (req_method == "GET")
    request_.method_ = request::GET;
  else if (req_method == "PUT")
    request_.method_ = request::PUT;
  else if (req_method == "POST")
    request_.method_ = request::POST;
  request_.uri_ = req_uri;
  request_.version_ = "HTTP/"+std::to_string(http_version_major)+"."+std::to_string(http_version_minor);

  request_.body_ = std::string(data_, bytes_transferred);

  for (auto it = req_headers.begin(); it != req_headers.end(); ++it)
  {
    request_.headers_.insert(std::pair<std::string, std::string> (it->name, it->value));
  }
}
