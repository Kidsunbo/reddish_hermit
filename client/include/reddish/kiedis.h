#pragma once

#include <common/network/connection.h>
#include <common/utils/types.h>
#include <cstddef>

namespace reddish
{
    class KiedisClient{
        common::network::Connection conn;

        public:
        KiedisClient(boost::asio::any_io_executor& ctx, int timeout = 0);
        KiedisClient(boost::asio::io_context& ctx, int timeout = 0);
        KiedisClient(boost::asio::io_context&& ctx, int timeout = 0);

        common::utils::AsyncResult<std::size_t> get();
        common::utils::AsyncResult<std::size_t> set();
    };
} // namespace reddish
