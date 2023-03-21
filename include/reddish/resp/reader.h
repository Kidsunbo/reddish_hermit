#ifndef REDDISH_HERMIT_RESP_READER_H
#define REDDISH_HERMIT_RESP_READER_H

#include "reddish/common/return_type.h"
#include "reddish/network/connection.h"

namespace reddish::resp
{
    /*
    Reader takes responsibility to read a whole reply from connection.
    And then feed the reply to Result in resp folder.
    */
    class Reader
    {

        std::string buf_string;

        std::int64_t index_offset;

        boost::asio::dynamic_string_buffer<std::string::value_type, std::string::traits_type, std::string::allocator_type> buf;

        AsyncResult<std::int64_t> read_whole_bulk_string(network::Connection &conn);

        AsyncResult<std::int64_t> read_whole_array(network::Connection &conn);

        AsyncResult<std::int64_t> read_whole_simple(network::Connection &conn);

    public:
        explicit Reader();

        AsyncResult<std::string> read_whole_reply(network::Connection &conn);
    };
} // namespace reddish::resp

#endif