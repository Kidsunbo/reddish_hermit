#ifndef REDDISH_HERMIT_REDIS_H
#define REDDISH_HERMIT_REDIS_H

#include "network/connection.h"

namespace reddish
{
    class RedisClient{
        network::Connection conn;

        public:
        RedisClient(boost::asio::any_io_executor& ctx, int timeout = 0);
        

    };
} // namespace reddish


#endif