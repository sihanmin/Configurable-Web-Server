//
// reply.hpp
// ~~~~~~~~~
// Adapted from https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp03/http/server/reply.hpp
//

#ifndef Response_h
#define Response_h

#include <string>
#include <boost/asio.hpp>
#include <map>


/// A reply to be sent to a client.
struct reply
{
  /// The status of the reply.
  enum StatusCode
  {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } code_;

  /// The headers to be included in the reply.
  std::map<std::string, std::string> headers_;


  /// The content to be sent in the reply.
  std::string body_;
  
};

class response_generator
{
public:
  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  static std::vector<boost::asio::const_buffer> to_buffers(reply reply_);

  /// Get a stock reply.
  static reply stock_reply(reply::StatusCode status);
};

#endif 