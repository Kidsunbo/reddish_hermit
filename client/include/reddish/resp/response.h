#ifndef REDDISH_HERMIT_RESP_RESPONSE_H
#define REDDISH_HERMIT_RESP_RESPONSE_H

#include "reddish/network/connection.h"
#include "result.h"
#include <string>

namespace reddish::resp {
    class Response {
        std::string raw_response;
        network::Connection conn;

    private:
        void fill_whole_raw_response();

    public:
        explicit Response(network::Connection conn);
    };
} // namespace reddish::resp

#endif