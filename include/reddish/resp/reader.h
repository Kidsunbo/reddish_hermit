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
    class Reader{
        public:
        static AsyncResult<std::string> read_whole_reply(network::Connection& conn); 
    };
} // namespace reddish::resp


#endif