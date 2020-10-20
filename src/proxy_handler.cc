#include "proxy_handler.h"
#include <stdlib.h>
#include <fstream>
// #include <thread> 
// #include <chrono> 

std::shared_ptr<request_handler> proxy_handler::Init(const std::string& path_prefix, const NginxConfig& config) 
{
  proxy_handler* handler = new proxy_handler;
  auto statement = config.statements_.begin();
  handler->dest_ = (*statement)->tokens_[1];
  handler->prefix_ = path_prefix;
  return std::shared_ptr<request_handler>(handler);
}

reply proxy_handler::handleRequest(request& request_)
{
	int rand_num;
	rand_num = rand() % 10000000 + 1;
	// Construct URI sent to destination
	std::string request_path = request_.uri_;
	std::string uri_to_dest = dest_ + request_path;
	if(uri_to_dest[uri_to_dest.size()-1]=='/') {
		uri_to_dest = uri_to_dest.substr(0,uri_to_dest.size()-1);
	}

	// CURL, get destinations' response, save headers and body into different local files.
  std::string body_file = "temp_file";
	body_file += std::to_string(rand_num);
	std::string header_file = "curl_headers" + std::to_string(rand_num);
	std::string curl_get_headers = "curl -s -L --output " + header_file + " --head \"" + uri_to_dest + "\"";
	std::string curl_get_body = "curl -s -L --output " + body_file + " \"" + uri_to_dest + "\"";
	system(curl_get_headers.c_str());
  system(curl_get_body.c_str());
	
	// Read saved header and body 
	std::ifstream h(header_file, std::ios::in);
	std::string headers;
	char c;
	while (h.get(c)) headers += c;
	h.close();
	
	std::ifstream b(body_file, std::ios::in | std::ios::binary);
	std::string body;
	while (b.get(c)) body += c;
	b.close();

	std::cerr << headers << std::endl;
	// Construct reply to client 
	reply reply_;
	get_reply_code(headers, reply_);
	modify_html(body);
	reply_.body_ = body;
	reply_.headers_.insert({"Content-Length", std::to_string(body.length())});
	reply_.headers_.insert({"Content-Type", get_content_type(headers)});

	// Remove temporary headers and body files
  	std::remove(header_file.c_str());
  	std::remove(body_file.c_str());

	return reply_;
}

std::string get_content_type(std::string headers) {
	int pos = headers.find("Content-Type: ");
	int endpos = pos + 14;
	int end   = headers.find("\r\n", pos);
	std::string content = headers.substr(endpos, end - endpos);
	return content;
}

void get_reply_code(std::string headers, reply& reply_) {
  int start = headers.rfind("HTTP/1.1 ");
  int end   = headers.find("\r\n", start);
  std::string http_status = headers.substr(start+9, end - start - 9);
  if (http_status == "200 OK")
    reply_.code_ = reply::ok;
  else if (http_status == "201 Created")
    reply_.code_ = reply::created;
  else if (http_status == "202 Accepted")
    reply_.code_ = reply::accepted;
  else if (http_status == "204 No Content")
    reply_.code_ = reply::no_content;
  else if (http_status == "300 Multiple Choices")
    reply_.code_ = reply::multiple_choices;
  else if (http_status == "304 Not Modified")
    reply_.code_ = reply::not_modified;
  else if (http_status == "400 Bad Request")
    reply_.code_ = reply::bad_request;
  else if (http_status == "401 Unauthorized")
    reply_.code_ = reply::unauthorized;
  else if (http_status == "403 Forbidden")
    reply_.code_ = reply::forbidden;
  else if (http_status == "404 Not Found")
    reply_.code_ = reply::not_found;
  else if (http_status == "500 Internal Server Error")
    reply_.code_ = reply::internal_server_error;
  else if (http_status == "501 Not Implemented")
    reply_.code_ = reply::not_implemented;
  else if (http_status == "502 Bad Gateway")
    reply_.code_ = reply::bad_gateway;
  else if (http_status == "503 Service Unavailable")
    reply_.code_ = reply::service_unavailable;
  else
    reply_.code_ = reply::internal_server_error;
}

void proxy_handler::modify_html(std::string& body) {
	size_t href_pos;
	size_t src_pos;
	int start = 0;
	while ((href_pos = body.find("href=\"", start)) != std::string::npos) {
		if (body[href_pos+6] == '/') {
			size_t next_quotation_mark = body.find("\"", href_pos+6);
			size_t uri_length = next_quotation_mark-(href_pos+6);
			body.replace(href_pos+6, uri_length, prefix_ + body.substr(href_pos+6, uri_length));
		}

		start = href_pos + 6;
	}

	start = 0;
	while ((src_pos = body.find("src=\"", start)) != std::string::npos) {
		if (body[src_pos+5] == '/') {
			size_t next_quotation_mark = body.find("\"", src_pos+5);
			size_t uri_length = next_quotation_mark-(src_pos+5);
			body.replace(src_pos+5, uri_length, prefix_ + body.substr(src_pos+5, uri_length));
		}

		start = src_pos + 5;
	}
}
