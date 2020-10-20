#include <cstdlib>
#include <iostream>
#include "health_handler.h"

reply health_handler::handleRequest(request& request_)
{
	reply reply_;
  reply_.code_ = reply::ok;
  reply_.body_ =  "ok";
  reply_.headers_.insert({"Content-Length",
    std::to_string(reply_.body_.length())});
  reply_.headers_.insert({"Content-Type", "text/plain"});
  return reply_;
}
