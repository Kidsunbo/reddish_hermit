#ifndef REDDISH_HERMIT_RESP_READER_H
#define REDDISH_HERMIT_RESP_READER_H

#include "reddish/common/return_type.h"
#include "reddish/network/connection.h"

namespace reddish::resp
{
    /*
    OnceReader takes responsibility to read a whole reply from connection.
    And then feed the reply to Result in resp folder. It should be only used once.
    */
    class OnceReader
    {
        std::string result;

        std::string buf_string;

        Buffer buf;

        AsyncResult<void> read_whole_bulk_string(network::Connection &conn);

        AsyncResult<void> read_whole_array(network::Connection &conn);

        AsyncResult<void> read_whole_simple(network::Connection &conn);

    public:
        explicit OnceReader();

        AsyncResult<std::string> read_whole_reply(network::Connection &conn);
    };
} // namespace reddish::resp

#endif