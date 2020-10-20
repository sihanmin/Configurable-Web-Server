#include "gtest/gtest.h"
#include "dispatcher.h"
#include "config_parser.h"

#include <iostream>

class DispatcherTestF : public ::testing::Test
{
protected:
  dispatcher *dispatcher_;
  NginxConfigParser parser;
  NginxConfig config;

  std::vector<std::string> paths;

  std::vector<std::string> separate_path(std::string path) {
    return dispatcher_->separate_path(path);
  }

  std::map<std::string, std::pair<std::shared_ptr<request_handler>, std::string>>
    get_handler_map() { return dispatcher_->handler_map;}
};

TEST_F(DispatcherTestF, Construction)
{
  parser.Parse("test_config/test_config", &config);
  dispatcher_ = new dispatcher(config);
  std::map<std::string, std::pair<std::shared_ptr<request_handler>, std::string>> 
    handler_map = get_handler_map();
	
  EXPECT_EQ(handler_map["/echo"].second, "EchoHandler");
  EXPECT_EQ(handler_map["/echo2"].second, "EchoHandler");
  EXPECT_EQ(handler_map["/static"].second, "StaticHandler");
  EXPECT_EQ(handler_map["/status"].second, "StatusHandler");
  EXPECT_EQ(handler_map["NotFoundHandler"].second, "NotFoundHandler");
  EXPECT_EQ(handler_map["BadRequestHandler"].second, "BadRequestHandler");
}

TEST_F(DispatcherTestF, SeperatePath1)
{
  dispatcher_ = new dispatcher(config);
  std::string path = "";
  paths = separate_path(path);
  EXPECT_EQ(paths.empty(),true);
}

TEST_F(DispatcherTestF, SeperatePath2)
{
  dispatcher_ = new dispatcher(config);
  std::string path = "/static/foo/index.html";
  paths = separate_path(path);
  EXPECT_EQ(paths.size(),3);
  EXPECT_EQ(paths[0],"/static");
  EXPECT_EQ(paths[1],"/foo");
  EXPECT_EQ(paths[2],"/index.html");
}

TEST_F(DispatcherTestF, SeperatePath3)
{
  dispatcher_ = new dispatcher(config);
  std::string path = "/static2";
  paths = separate_path(path);
  EXPECT_EQ(paths.size(),1);
  EXPECT_EQ(paths[0],"/static2");
}

TEST_F(DispatcherTestF, CallHandler1)
{
  request request_;
  reply reply_;
  parser.Parse("test_config/test_config", &config);
  dispatcher_ = new dispatcher(config);

  request_.uri_ = "invalid_request";
  reply_ = dispatcher_->call_handler(request_);
  EXPECT_EQ(reply_.code_, reply::bad_request);

  request_.uri_ = "/static/../parent_folder_file";
  reply_ = dispatcher_->call_handler(request_);
  EXPECT_EQ(reply_.code_, reply::bad_request);
}

TEST_F(DispatcherTestF, CallHandler2)
{
  request request_;
  reply reply_;
  parser.Parse("test_config/test_config", &config);
  dispatcher_ = new dispatcher(config);

  request_.uri_ = "/echo/something";
  request_.body_ = "Whatever this is to be echoed back";
  reply_ = dispatcher_->call_handler(request_);
  EXPECT_EQ(reply_.code_, reply::ok);
  EXPECT_EQ(reply_.body_, request_.body_);
}

TEST_F(DispatcherTestF, CallHandler3)
{
  request request_;
  reply reply_;
  parser.Parse("test_config/test_config", &config);
  dispatcher_ = new dispatcher(config);

  request_.uri_ = "/static";
  reply_ = dispatcher_->call_handler(request_);
  EXPECT_EQ(request_.uri_, "/index.html");
  EXPECT_EQ(reply_.code_, reply::ok);
  EXPECT_EQ(reply_.headers_["Content-Type"], "text/html");
}

TEST_F(DispatcherTestF, CallHandler4)
{
  request request_;
  reply reply_;
  parser.Parse("test_config/test_config", &config);
  dispatcher_ = new dispatcher(config);

  request_.uri_ = "/static/file_not_exist.txt";
  reply_ = dispatcher_->call_handler(request_);
  EXPECT_EQ(request_.uri_, request_.uri_);
  EXPECT_EQ(reply_.code_, reply::not_found);
  EXPECT_EQ(reply_.headers_["Content-Type"], "text/html");
}

TEST_F(DispatcherTestF, CallHandler5)
{
  request request_;
  reply reply_;
  parser.Parse("test_config/test_config", &config);
  dispatcher_ = new dispatcher(config);

  request_.uri_ = "/path_not_exist";
  reply_ = dispatcher_->call_handler(request_);
  EXPECT_EQ(reply_.code_, reply::not_found);
  EXPECT_EQ(reply_.headers_["Content-Type"], "text/html");
}

