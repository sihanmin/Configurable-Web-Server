#ifndef request_handler_h
#define request_handler_h

#include <cstdlib>
#include <iostream>
#include "reply.h"
#include "request.h"
#include "config_parser.h"

const std::string static_handler_ = "StaticHandler";
const std::string echo_handler_ = "EchoHandler";
const std::string status_handler_ = "StatusHandler";
const std::string not_found_handler_ = "NotFoundHandler";
const std::string bad_request_handler_ = "BadRequestHandler";
const std::string proxy_handler_ = "ProxyHandler";
const std::string health_handler_ = "HealthHandler";
const std::string game_handler_ = "GameHandler";
const short max_length = 1024;

class request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config);
	virtual reply handleRequest(request& request_) = 0;
	
};

#endif
