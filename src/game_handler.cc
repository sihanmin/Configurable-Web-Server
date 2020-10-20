#include <iostream>
#include "game_handler.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.h"
#include "reply.h"

std::string urlDecode(std::string &eString);


std::shared_ptr<request_handler> game_handler::Init(const std::string& path_prefix, const NginxConfig& config) 
{
  game_handler* handler = new game_handler;
  auto statement = config.statements_.begin();
  handler->doc_root_ = (*statement)->tokens_[1];
  handler->prefix_ = path_prefix;
  return std::shared_ptr<request_handler>(handler);
}

reply game_handler::handleRequest(request& request_)
{
  if (request_.uri_ == "/customize"){
    return handleCustomize(request_);
  }
  std::string request_path = doc_root_ + "/index.html";

  std::string extension = "html";

  // Open the file to send back.
  std::ifstream is(request_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
    return response_generator::stock_reply(reply::not_found);

  
  reply reply_;
  reply_.code_ = reply::ok;
	is.seekg (0, is.end);
  int length = is.tellg();
  is.seekg (0, is.beg);
  char buf[length];
	is.read(buf, length);

	reply_.body_.append(buf, length);

  // serve customized game version
  if (request_.uri_.substr(0,11) == "/customize/") {
    std::string file_name = request_.uri_.substr(11,request_.uri_.length()-11);
    file_name += ".png";

    // check if corresponding customized file exist in local directory
    std::string loc_path = doc_root_ + "/assets/customize/" + file_name;
    std::ifstream infile(loc_path);
    if (!infile.good()) {
      return response_generator::stock_reply(reply::not_found);
    }
    modify_game(reply_.body_, file_name);
  }

  // all other paths serves default game
  reply_.headers_.insert({"Content-Length", 
    boost::lexical_cast<std::string>(reply_.body_.size())});
  reply_.headers_.insert({"Content-Type", 
    http::server::mime_types::extension_to_type(extension)});
  return reply_;
}

void game_handler::modify_game(std::string& body, std::string pic_name) {
  size_t pic_pos;
  int start = 0;

  // substitute game asset path to customized image in .html game file
  while ((pic_pos = body.find("default_100_percent/100-offline-sprite.png", start)) != std::string::npos) {
    size_t next_quotation_mark = body.find("\"", pic_pos);
    size_t uri_length = next_quotation_mark - pic_pos;
    body.replace(pic_pos, uri_length, "customize/" + pic_name);

    start = pic_pos+1;
  }

}

reply game_handler::handleCustomize(request& request_) {
  std::string rand_num = std::to_string(rand());

  std::string square_name = "square_" + rand_num + ".png";
  std::string stretch_name = "stretch_" + rand_num + ".png";

  std::string customize_id = "No" + rand_num;
  std::string pic_output = customize_id +".png";
  std::string loc_root = doc_root_ + "/assets/customize/";

  std::string square_content;
  std::string stretch_content;
  // parse json image content
  bool parse_result =  parse_image(request_.body_, square_content, stretch_content);
  if (!parse_result) {
    return response_generator::stock_reply(reply::not_found);
  }

  // decode base64 and save image to local path
  bool save_result =  save_image(square_content, loc_root + square_name);
  if (!save_result) {
    return response_generator::stock_reply(reply::not_found);
  }
  save_result =  save_image(stretch_content, loc_root + stretch_name);
  if (!save_result) {
    return response_generator::stock_reply(reply::not_found);
  }
  
  // use runner element to modify game assets
  pic_modifier modifier_;
  int result  = modifier_.modify_pic(loc_root+square_name, 
    loc_root+stretch_name, pic_output, loc_root);
  if (result != 0) {
    return response_generator::stock_reply(reply::not_found);
  }
  std::remove((loc_root+square_name).c_str());
  std::remove((loc_root+stretch_name).c_str());

  reply reply_;
  reply_.code_ = reply::ok;
  reply_.body_.append(customize_id);

  reply_.headers_.insert({"Content-Length", 
    boost::lexical_cast<std::string>(reply_.body_.size())});
  reply_.headers_.insert({"Content-Type", "text/plain"});
  reply_.headers_.insert({"Customize-Id", customize_id});

  return reply_;
  
}


bool game_handler::parse_image(std::string& body,
  std::string& square, std::string& stretch) {
  size_t pic_pos;
  int start = 0;
  bool is_square = true;

  // find and extract two instances of base64 image string
  while ((pic_pos = body.find("base64", start)) != std::string::npos) {
    size_t next_comma = body.find(",", pic_pos);
    size_t next_quotation_mark = body.find("\"", pic_pos);
    size_t img_length = next_quotation_mark - next_comma - 1;
    if (is_square) {
      square.append(body.substr(next_comma+1, img_length));
      is_square = false;
    }
    else {
      stretch = body.substr(next_comma+1, img_length);
      return true;
    }
    start = pic_pos+1;
  }
  return false;

}

bool game_handler::save_image(std::string& base64_img, std::string loc_path){
    // image decoding
    std::string image = base64_decode(base64_img);

    // write to local path
    FILE * File = NULL;
    File = fopen(loc_path.c_str(),"wb");
    if(File == NULL){
      return false;
    }
    if(fwrite(image.c_str(),image.size(),1,File) != image.size())
    {
        fclose(File);
    }

    fclose(File);
    return true;
}


