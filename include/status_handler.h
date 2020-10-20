#ifndef status_handler_h
#define status_handler_h

#include <cstdlib>
#include <iostream>
#include "request_parser.h"
#include "request_handler.h"
#include "reply.h"
#include "dispatcher.h"

class status_handler : public request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config) 
		{return std::shared_ptr<request_handler>(new status_handler);}
	virtual reply handleRequest(request& request_);
};

#endif
