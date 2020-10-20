#include <cstdlib>
#include <iostream>
#include "echo_handler.h"

reply echo_handler::handleRequest(request& request_)
{
	reply reply_;
	size_t bytes_transferred = request_.body_.length();
	reply_.code_ = reply::StatusCode::ok;
	reply_.body_ = request_.body_;
	reply_.headers_.insert({"Content-Length", std::to_string(bytes_transferred)});
	reply_.headers_.insert({"Content-Type", "text/plain"});
	return reply_;
}
