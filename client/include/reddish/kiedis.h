#ifndef REDDISH_HERMIT_KIEDIS_H
#define REDDISH_HERMIT_KIEDIS_H

#include "network/connection.h"
#include "reddish/common/return_type.h"
#include <cstddef>

namespace reddish
{
    class KiedisClient{
        network::Connection conn;

        public:
        KiedisClient(boost::asio::any_io_executor& ctx, int timeout = 0);
        KiedisClient(boost::asio::io_context& ctx, int timeout = 0);
        KiedisClient(boost::asio::io_context&& ctx, int timeout = 0);

        AsyncResult<std::size_t> get();
        AsyncResult<std::size_t> set();
    };
} // namespace reddish


#endif