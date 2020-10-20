#include "dispatcher.h"
#include <iostream>

std::map<std::string, std::pair<std::shared_ptr<request_handler>, std::string>> dispatcher::handler_map;
unsigned int dispatcher::n_requests = 0;
std::vector<std::pair<std::string, int>> dispatcher::uri_requested;

std::shared_mutex dispatcher::log_mtx;
std::shared_mutex dispatcher::uri_mtx;

dispatcher::dispatcher(const NginxConfig& config)
	: config(config)
{	
	std::shared_ptr<request_handler> handler;
	for (auto statement = config.statements_.begin(); statement != config.statements_.end(); statement++) {
		if ((*statement)->tokens_.size() == 3 && (*statement)->tokens_[0] == "location") {
			std::string handler_type = (*statement)->tokens_[2];
			std::string path_prefix = (*statement)->tokens_[1];
			//BOOST_LOG_TRIVIAL(info) << handler_type << path_prefix;
			handler = createHandler(handler_type, path_prefix, *((*statement)->child_block_.get()));
			handler_map.insert(
				std::make_pair(path_prefix, 
					std::make_pair(handler, handler_type)));
		}
	}
	handler = createHandler(not_found_handler_, "/", config);
	handler_map.insert(
		std::make_pair(not_found_handler_, 
			std::make_pair(handler, not_found_handler_)));

	handler = createHandler(bad_request_handler_, "", config);
	handler_map.insert(
		std::make_pair(bad_request_handler_, 
			std::make_pair(handler, bad_request_handler_)));

	// Sanity check on path configuration can be added here   
}


std::shared_ptr<request_handler> dispatcher::createHandler(const std::string& name, const std::string& path_prefix, const NginxConfig& config)
{
	if (name == static_handler_) {
		return static_handler::Init(path_prefix, config);
	}
	else if (name == echo_handler_) {
		return echo_handler::Init(path_prefix, config);
	}
	else if (name == not_found_handler_) {
		return not_found_handler::Init(path_prefix, config);
	}
	else if (name == bad_request_handler_) {
		return bad_request_handler::Init(path_prefix, config);
	}
	else if (name == status_handler_) {
		return status_handler::Init(path_prefix, config);
	}
	else if (name == proxy_handler_) {
		return proxy_handler::Init(path_prefix, config);
	}
	else if (name == health_handler_) {
		return health_handler::Init(path_prefix, config);
	}
	else if (name == game_handler_) {
		return game_handler::Init(path_prefix, config);
	}
}

reply dispatcher::call_handler(request& request_)
{
	std::string original_path = request_.uri_;
	std::string path_ = request_.uri_;
	std::vector<std::string> paths = separate_path(path_);
	std::vector<std::string> possible_paths = form_possible_paths(paths);
	reply reply_;
	int index;
	if (paths.empty() || (path_[0] != '/'
      || path_.find("..") != std::string::npos))
	{
		reply_ = handler_map[bad_request_handler_].first->handleRequest(request_);
    log("error", "Bad request:\n" + request_.body_);
    log_metrics(reply_.code_, original_path, bad_request_handler_);
	}
	else if ((index = prefix_match(possible_paths)))
	{
		index -= 1;
		std::string request_path = path_.erase(0, possible_paths[index].length());
		if (request_path.length() == 0)
			request_path = "/";
		
		if (request_path[request_path.size() - 1] == '/' && handler_map[possible_paths[index]].second != "ProxyHandler")
		{
			request_path += "index.html";
		}
		
		request_.uri_ = request_path;
		reply_ = handler_map[possible_paths[index]].first->handleRequest(request_);
		if (reply_.code_ == reply::bad_request) {
      log("error", "Bad request:\n" + request_.body_);
		} else if (reply_.code_ == reply::not_found) {
      log("error", "Not found:\n" + request_.body_);
		} else {
      log("info", possible_paths[index] + " request:\n" + request_.body_);
		}
    log_metrics(reply_.code_, original_path, handler_map[possible_paths[index]].second);
	}
	else
	{
		reply_ = handler_map[not_found_handler_].first->handleRequest(request_);
    log("error", "Not found:\n" + request_.body_);
    log_metrics(reply_.code_, original_path, not_found_handler_);
	}
  
  uri_mtx.lock();
	n_requests++;
	uri_requested.push_back(std::make_pair(original_path, reply_.code_));
  uri_mtx.unlock();
  
	return reply_;
}

std::vector<std::string> dispatcher::separate_path(std::string path)
{
  std::vector<std::string> paths;
  if (path.empty() || path[0] != '/'
      || path.find("..") != std::string::npos)
    return paths;
  int substr_start_index = 0;
	int counter = 0;
  for (size_t i = 1; i < path.length(); i++)
  {
		counter++;
    if (path[i] == '/')
    {
      paths.push_back(path.substr(substr_start_index,counter));
			substr_start_index = i;
			counter = 0;
    }
  }
  paths.push_back(path.substr(substr_start_index, path.length()));
	/*
	for (size_t i = 0; i < paths.size(); i++)
	{
		std::cout << paths.at(i) << std::endl;
	}
	*/	
  return paths;
}


std::vector<std::string> dispatcher::form_possible_paths(std::vector<std::string> paths)
{
	std::string str;
	std::vector<std::string> possible_prefix;
	for(int i=0; i < paths.size(); i++)
	{
    str.append(paths.at(i));
		possible_prefix.push_back(str);
	}
	return possible_prefix;
}

int dispatcher::prefix_match(std::vector<std::string> possible_prefix)
{
	for (int i = possible_prefix.size() - 1; i >= 0; i--)
	{
		if (handler_map.find(possible_prefix.at(i)) != handler_map.end())
			return i + 1;
	}
	return 0;
}

void dispatcher::log(std::string level, std::string message) {
  log_mtx.lock();
  if (level == "info") {
    BOOST_LOG_TRIVIAL(info) << message;
  }
  else if (level == "error") {
    BOOST_LOG_TRIVIAL(error) << message;
  }
  log_mtx.unlock();
}

void dispatcher::log_metrics(reply::StatusCode code, std::string path, std::string name) {
  log_mtx.lock();
  BOOST_LOG_TRIVIAL(info) << "Response metrics for last request:\n" << metric::metric << metric::code << code << "\n" << metric::metric << metric::path << path << "\n" << metric::metric << metric::name << name << "\n";
  log_mtx.unlock();
}
