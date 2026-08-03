#pragma once

#include <common/network/connection.h>
#include <common/protocol/resp_value.h>
#include <common/utils/types.h>

#include <cstdint>
#include <string>
#include <vector>

namespace reddish::common::protocol {

    class RESPWriter {
        network::Connection& conn;

    public:
        RESPWriter(network::Connection& conn)
            : conn(conn)
        {
        }

        utils::AsyncResult<std::size_t> write(const RESPValue& value);

        utils::AsyncResult<std::size_t> write_simple_string(std::string value);

        utils::AsyncResult<std::size_t> write_bulk_string(std::string value);

        utils::AsyncResult<std::size_t> write_null_bulk_string();

        utils::AsyncResult<std::size_t> write_array(std::vector<std::string> value);

        utils::AsyncResult<std::size_t> write_error(std::string code, std::string message);

        utils::AsyncResult<std::size_t> write_integer(std::int64_t value);
    };

}
