#pragma once

#include <boost/asio.hpp>
#include <common/network/connection.h>
#include <common/protocol/reader.h>
#include <common/protocol/writer.h>

class ConnectionHandler {
    reddish::common::protocol::RESPReader reader;
    reddish::common::protocol::RESPWriter writer;

public:
    ConnectionHandler(reddish::common::network::Connection&& conn)
        : reader(conn)
        , writer(conn)
    {
    }

    boost::asio::awaitable<void> handle_connection();

    boost::asio::awaitable<void> handle_unsupported_command();

    boost::asio::awaitable<void> handle_request(const std::vector<std::string>& value);
};
