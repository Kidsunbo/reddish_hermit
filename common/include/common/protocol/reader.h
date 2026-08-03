#pragma once

#include <common/network/connection.h>
#include <common/protocol/protocol.h>
#include <common/utils/types.h>

#include <cstdint>
#include <string>

namespace reddish::common::protocol {

    class RESPReader final {
        network::Connection& conn;

    private:
        utils::AsyncResult<std::string> read_line();

        utils::AsyncResult<std::string> read_exact(std::uint64_t size);

    public:
        explicit RESPReader(network::Connection& conn)
            : conn(conn)
        {
        }

        utils::AsyncResult<RESPValue> read();
    };

}
