#include <cstdlib>
#include <iostream>
#include "not_found_handler.h"

reply not_found_handler::handleRequest(request& request_)
{
	return response_generator::stock_reply(reply::not_found);
}