#ifndef bad_request_handler_h
#define bad_request_handler_h

#include <cstdlib>
#include <iostream>
#include "request_handler.h"

class bad_request_handler : public request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config) 
		{return std::shared_ptr<request_handler>(new bad_request_handler);}
	virtual reply handleRequest(request& request_);
};


#endif
