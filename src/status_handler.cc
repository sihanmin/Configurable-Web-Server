#include <cstdlib>
#include <iostream>
#include "status_handler.h"


reply status_handler::handleRequest(request& request_)
{
	reply reply_;
  reply_.code_ = reply::ok;
  
  std::string request_status;
  dispatcher::uri_mtx.lock_shared();
  for (auto it = dispatcher::uri_requested.begin(); it != dispatcher::uri_requested.end(); it++) {
    request_status += "<tr><td>" + it->first +"</td><td>" + std::to_string(it->second) + "</td></tr>";
  }
  dispatcher::uri_mtx.unlock_shared();
  
  std::string handler_status;
  for (auto it = dispatcher::handler_map.begin(); it != dispatcher::handler_map.end(); it++) {
    if (it->second.second == not_found_handler_ || it->second.second == bad_request_handler_) {
      handler_status += "<tr><td>N/A</td><td>" +  it->second.second + "</td></tr>";
    }
    else {
      handler_status += "<tr><td>" + it->first +"</td><td>" +  it->second.second + "</td></tr>";
    }
  }
  
  std::string full_content = "<html><head>""<title>Server Status</title></head>"
  "<body><h2>Total number of requests</h1><div>"+ std::to_string(dispatcher::n_requests) +"</div>"
  "<h2>Requests</h2>"
  "<table>"
  "<tr><th>URL Requested</th><th>Return Code</th></tr>" + request_status + "</table>"
  "<h2>Request Handlers</h2>"
  "<table>"
  "<tr><th>URL Prefix</th><th>Handler</th></tr>" + handler_status +  "</table>"
  "</body>"
  "</html>";
  
  reply_.body_ =  full_content;
  reply_.headers_.insert({"Content-Length", 
    std::to_string(reply_.body_.length())});
  reply_.headers_.insert({"Content-Type", "text/html"});
  return reply_;  
}
