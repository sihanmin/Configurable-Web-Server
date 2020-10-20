#ifndef not_found_handler_h
#define not_found_handler_h

#include <cstdlib>
#include <iostream>

#include "request_handler.h"

class not_found_handler : public request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config) 
		{return std::shared_ptr<request_handler>(new not_found_handler);}
	virtual reply handleRequest(request& request_);
};


#endif
