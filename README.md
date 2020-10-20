# APTX Web Server

## 1. Source Code Layout

```
                            +----------------------------+
             +--------------+        server_main.cc      +-------------------------+
  get parsed |              +--------------+-------------+      init a dispatcher  |
  config from|          init a server      |                    according to config|
             |          according to config|                                       |
             V                             V                                       V
  +----------+----------+         +--------+--------+                +-------------+--------------+
  |    config_parser.h  |         |    server.h     |                |      dispatcher.h          |
  +---------------------+         +--------+--------+                +---+---------+--------------+
                                           |                             Λ         |             
                        create a session   |                             |         |get reply     
                        waiting for request|                    get reply|         |from           
                                           V                         from|         V             
  +------------------+             +-------+---------+                   |  +------+--------------------+
  | request_parser.h +<------------+    session.h    +-------------------+  | request_handler.h         |
  +------------------+ get parsed  +-----------------+                      | +-- echo_handler.h        |
                       request from                                         | +-- static_handler.h      |
                                                                            | +-- status_handler.h      |
                                                                            | +-- not_found_handler.h   |
                                                                            | +-- bad_request_handler.h |
                                                                            +---------------------------+
```
Other header files include:
* `request.h`: Defines structure of a HTTP request.
* `reqly.h`: Defines structure of a HTTP reply.
* `mime_types.h`: Defines a method for converting file extensions into MIME types.

## 2. Build, Test and Run

### Build

Run the following commands from root directory:
```bash
$ mkdir -p build
$ cd build
$ cmake ..
$ make
```

### Test

Run `make test` from `build` directory created upon building. This will run all the unit tests together with the integration test. Or, if you only want the integration test, run `../tests/integration_test.sh` from `build` directory.

### Run

Run the following command from `build` directory:
```bash
bin/server ../tests/config
```
You can supply your own config file through the argument.


## 3. Adding a New Request Handler

We will use `StatusHandler` as an example of how to add a new type of request handler to the server. All paths in this section are relative to the root directory.

1. Define the `StatusHandler` class in `include/status_handler.h`.
```C++
...
#include "request_handler.h" // include the header file for base class
class status_handler : public request_handler // inherit the base class publicly
{
public: // be sure to declare the two public methods in the base class with the same interface
static std::shared_ptr<request_handler> Init(const NginxConfig& config) // if the init method is heavy, implement in the .cc file instead
    {return std::shared_ptr<request_handler>(new status_handler);}
  virtual reply handleRequest(request& request_);
};
...
```

2. Implement its member functions in `src/status_handler.cc`. While doing this, please make sure all of its member functions are thread-safe since the server is multithreaded.
```C++
...
#include "status_handler.h" // include the header file for this class
reply status_handler::handleRequest(request& request_) // returns a reply object that will be sent to the client
{
  reply reply_; // declare the reply to be returned
  reply_.code_ = reply::ok; // fill out the return code
  
  // fill out the content field
  std::string request_status;
  for (auto it = dispatcher::uri_requested.begin(); it != dispatcher::uri_requested.end(); it++) {
    request_status += "<tr><td>" + it->first +"</td><td>" + std::to_string(it->second) + "</td></tr>";
  }
  
  std::string handler_status;
  for (auto it = dispatcher::handler_map.begin(); it != dispatcher::handler_map.end(); it++) {
    if (it->second.second == not_found_handler_ || it->second.second == bad_request_handler_) {
      handler_status += "<tr><td>N/A</td><td>" +  it->second.second + "</td></tr>";
    }
    else {
      handler_status += "<tr><td>" + it->first +"</td><td>" +  it->second.second + "</td></tr>";
    }
  }
  
  std::string full_content = "<html><head>""<title>Server Status</title></head>"
  "<body><h2>Total number of requests</h1><div>"+ std::to_string(dispatcher::n_requests) +"</div>"
  "<h2>Requests</h2>"
  "<table>"
  "<tr><th>URL Requested</th><th>Return Code</th></tr>" + request_status + "</table>"
  "<h2>Request Handlers</h2>"
  "<table>"
  "<tr><th>URL Prefix</th><th>Handler</th></tr>" + handler_status +  "</table>"
  "</body>"
  "</html>";
  
  reply_.body_ =  full_content;
  
  //fill out the header field
  reply_.headers_.insert({"Content-Length", 
    std::to_string(reply_.body_.length())});
  reply_.headers_.insert({"Content-Type", "text/html"});
  return reply_;  
}
...
```

3. Include the new header file in `include/dispatcher.h`.
```C++
...
#include "config_parser.h"
#include "echo_handler.h"
#include "not_found_handler.h"
#include "bad_request_handler.h"
#include "request_handler.h"
#include "static_handler.h"
#include "status_handler.h" // include the header file here
...
```

4. Come up with a string identifier for the new class. Define it as a constant in `include/request_handler.h`.
```C++
...
const std::string static_handler_ = "StaticHandler";
const std::string echo_handler_ = "EchoHandler";
const std::string not_found_handler_ = "NotFoundHandler";
const std::string bad_request_handler_ = "BadRequestHandler";
const std::string status_handler_ = "StatusHandler"; // define the identifier here
...
```

5. In the implementation of `dispatcher::createHandler()` from `src/dispatcher.cc`, add codes to call `status_handler::Init()` if its identifier is supplied as the `name` argument.
```C++
...
std::shared_ptr<request_handler> dispatcher::createHandler(const std::string& name, const NginxConfig& config)
{
  if (name == static_handler_) {
    return static_handler::Init(config);
  }
  else if (name == echo_handler_) {
    return echo_handler::Init(config);
  }
  else if (name == not_found_handler_) {
    return not_found_handler::Init(config);
  }
  else if (name == bad_request_handler_) {
    return bad_request_handler::Init(config);
  }
  else if (name == status_handler_) { // simply add an else if statement here, similar to the existing handlers
    return status_handler::Init(config);
  }
}
...
```

6. In `CMakeLists.txt`, locate the line where we add libraries for all the request handlers. Add `src/status_handler.cc` to that statement.
```C++
...
add_library(request_handler_lib src/echo_handler.cc src/bad_request_handler.cc src/static_handler.cc src/not_found_handler.cc src/status_handler.cc src/mime_types.cc src/dispatcher.cc)
...
```

Now you should be able to build the server with your newly added request handelr. To verify it actually works, add config statement(s) referring to the string identifier of the new request handler in your configuration file. Like this:
```
...
location /status StatusHandler { # configure the status handler to handle all uri with prefix /status
} # note the string identifier defined in Step 4 must be used here
...
```
