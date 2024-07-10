#pragma once

#include "common/command/command.h"
#include "common/protocol/resp_data_type.h"
#include <common/network/connection.h>
#include <common/utils/types.h>
#include <string>

namespace reddish
{
    class KiedisClient{
        common::network::Connection conn;

        public:
        KiedisClient(boost::asio::any_io_executor& ctx, int timeout = 0);
        KiedisClient(boost::asio::io_context& ctx, int timeout = 0);
        KiedisClient(boost::asio::io_context&& ctx, int timeout = 0);

        common::utils::AsyncResult<common::protocol::KiedisSupportType> get(std::string_view key);
        template<typename T, typename... ARGS>
        common::utils::AsyncResult<void> set(std::string_view key, const T& value, ARGS... args){
            auto command = common::commands::Command<common::commands::CommandEnum::Set>::to_string(key, std::to_string(value), args...);
        }

    };
} // namespace reddish
