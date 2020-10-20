#ifndef echo_handler_h
#define echo_handler_h

#include <cstdlib>
#include <iostream>
#include "request_parser.h"
#include "request_handler.h"

class echo_handler : public request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config) 
		{return std::shared_ptr<request_handler>(new echo_handler);}
	virtual reply handleRequest(request& request_);
};

#endif