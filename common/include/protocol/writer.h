#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <string>
#include <vector>

class RESPWriter
{
    boost::asio::ip::tcp::socket& socket;

    private:
      boost::asio::awaitable<std::int64_t> write(std::string);

    public:
    RESPWriter(boost::asio::ip::tcp::socket& socket):socket(socket){}

    boost::asio::awaitable<std::int64_t> write_simple_string(std::string);

    boost::asio::awaitable<std::int64_t> write_bulk_string(std::string);

    boost::asio::awaitable<std::int64_t> write_null_bulk_string();

    boost::asio::awaitable<std::int64_t> write_array(std::vector<std::string>);

    boost::asio::awaitable<std::int64_t> write_error(std::string, std::string);

    boost::asio::awaitable<std::int64_t> write_integer(std::int64_t);
};