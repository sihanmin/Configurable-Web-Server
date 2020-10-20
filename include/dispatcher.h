#ifndef dispatcher_h
#define dispatcher_h

#include "config_parser.h"
#include "echo_handler.h"
#include "not_found_handler.h"
#include "bad_request_handler.h"
#include "request_handler.h"
#include "static_handler.h"
#include "status_handler.h"
#include "proxy_handler.h"
#include "health_handler.h"
#include "game_handler.h"
#include "reply.h"
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>
#include <mutex>
#include <shared_mutex>
#include <boost/log/trivial.hpp>

class status_handler;
class DispatcherTestF;

namespace metric {
  const std::string metric = "[ResponseMetrics]";
  const std::string code = "response_code:";
  const std::string path = "request_path:";
  const std::string name = "request_handler_name:";
}

class dispatcher
{
public:
	dispatcher(const NginxConfig& config);
	//std::shared_ptr<request_handler> createHandler(const std::string& name, const NginxConfig& config);

	reply call_handler(request& request_);
  friend class status_handler;
  friend class DispatcherTestF;
  
  void log(std::string level, std::string message);
  void log_metrics(reply::StatusCode code, std::string path, std::string name);

protected:
	/// separate path with "/" being the delimiter
	std::vector<std::string> separate_path(std::string path);
	std::vector<std::string> form_possible_paths(std::vector<std::string> paths);
	int prefix_match(std::vector<std::string> possible_prefix);
	std::shared_ptr<request_handler> createHandler(const std::string& name, const std::string& path_prefix, const NginxConfig& config);

	static std::map<std::string, std::pair<std::shared_ptr<request_handler>, std::string>> handler_map;
	const NginxConfig config;
  
	static unsigned int n_requests; // total number of requests received
	static std::vector<std::pair<std::string, int>> uri_requested; // pair of uri requested and return code
  
  static std::shared_mutex log_mtx;
  static std::shared_mutex uri_mtx;
};

#endif
