#ifndef REDDISH_HERMIT_REDIS_H
#define REDDISH_HERMIT_REDIS_H

#include "network/connection.h"
#include "reddish/common/return_type.h"
#include <cstddef>

namespace reddish
{
    class RedisClient{
        network::Connection conn;

        public:
        RedisClient(boost::asio::any_io_executor& ctx, int timeout = 0);
        RedisClient(boost::asio::io_context& ctx, int timeout = 0);
        RedisClient(boost::asio::io_context&& ctx, int timeout = 0);

        AsyncResult<std::size_t> get();
        AsyncResult<std::size_t> set();
    };
} // namespace reddish


#endif