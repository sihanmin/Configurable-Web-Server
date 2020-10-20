#ifndef static_handler_h
#define static_handler_h

#include <cstdlib>
#include <iostream>
#include <string>

#include "request.h"
#include "request_handler.h"
#include "dispatcher.h"
#include "utilities.h"

class static_handler : public request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config);
	virtual reply handleRequest(request& request_);

	std::string doc_root_;
};

#endif
