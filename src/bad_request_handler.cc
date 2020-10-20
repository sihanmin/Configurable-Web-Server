#include "bad_request_handler.h"

reply bad_request_handler::handleRequest(request& request_)
{
	return response_generator::stock_reply(reply::bad_request);
}