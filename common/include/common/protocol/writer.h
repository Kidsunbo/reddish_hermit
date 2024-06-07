#pragma once

#include "common/network/connection.h"
#include <boost/asio.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace reddish::common::protocol {

    class RESPWriter {
        network::Connection& conn;

    private:
        boost::asio::awaitable<std::int64_t> write(std::string);

    public:
        RESPWriter(network::Connection& conn)
            : conn(conn)
        {
        }

        boost::asio::awaitable<std::int64_t> write_simple_string(std::string);

        boost::asio::awaitable<std::int64_t> write_bulk_string(std::string);

        boost::asio::awaitable<std::int64_t> write_null_bulk_string();

        boost::asio::awaitable<std::int64_t> write_array(std::vector<std::string>);

        boost::asio::awaitable<std::int64_t> write_error(std::string, std::string);

        boost::asio::awaitable<std::int64_t> write_integer(std::int64_t);
    };
}