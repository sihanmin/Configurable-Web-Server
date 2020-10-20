#ifndef proxy_handler_h
#define proxy_handler_h

#include <cstdlib>
#include <iostream>
#include "request_handler.h"
#include "utilities.h"

class proxy_handler : public request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config);
	virtual reply handleRequest(request& request_);

	std::string dest_;
	std::string prefix_;
private:
	void modify_html(std::string& body);
};

std::string get_content_type(std::string headers);
void get_reply_code(std::string headers, reply& reply_);
// void issue_request(request& request_, std::string &headers, std::string &body, );
#endif
