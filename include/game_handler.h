#ifndef game_handler_h
#define game_handler_h

#include <cstdlib>
#include <iostream>
#include <string>

#include "request.h"
#include "request_handler.h"
#include "dispatcher.h"
#include "utilities.h"
#include "pic_modifier.h"
#include "base64.h"

class game_handler : public request_handler
{
public:
	static std::shared_ptr<request_handler> Init(const std::string& path_prefix, const NginxConfig& config);
	virtual reply handleRequest(request& request_);

	std::string doc_root_;
	std::string prefix_;
private:
	reply handleCustomize(request& request_);
	bool parse_image(std::string& body, 
		std::string& square, std::string& stretched);
	bool save_image(std::string& base64_img, std::string loc_path);
	void modify_game(std::string& body, std::string pic_name);
};

#endif