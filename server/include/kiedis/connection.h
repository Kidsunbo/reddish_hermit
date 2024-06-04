#pragma once

#include <protocol/reader.h>
#include <protocol/writer.h>
#include <boost/asio.hpp>

class ConnectionHandler
{
    RESPReader reader;
    RESPWriter writer;

  public:
    ConnectionHandler(boost::asio::ip::tcp::socket &&socket) : reader(socket), writer(socket)
    {
    }

    boost::asio::awaitable<void> handle_connection();

    boost::asio::awaitable<void> handle_unsupported_command();

    boost::asio::awaitable<void> handle_request(const std::vector<std::string> &value);
};