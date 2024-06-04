#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

class RESPReader final
{
    boost::asio::ip::tcp::socket& socket;
    std::string data;

  public:
    // for now, only resp2 proto is supported
    enum class ReplyType : char
    {
        Error = '-',
        SimpleString = '+',
        Integer = ':',
        BulkString = '$',
        Arrays = '*',
    };

  private:
    boost::asio::awaitable<std::string> read_line();

    boost::asio::awaitable<std::string> read_exact(std::int32_t size);

    // get the error if next item is of type error
    boost::asio::awaitable<std::tuple<std::string, std::string>> read_error(std::string previous_line);

    // get the simple string if next item is of type simple string
    boost::asio::awaitable<std::string> read_simple_string(std::string previous_line);

    // get the bulk string if next item is of type bulk string
    boost::asio::awaitable<std::string> read_bulk_string(std::string previous_line);

    // get the integer if next item is of type integer
    boost::asio::awaitable<std::int64_t> read_integer(std::string previous_line);

    // get the array if next item is of type integer.
    // Note that, for simplicity, it reads all the items in the array as string, users can parse afterwards.
    boost::asio::awaitable<std::vector<std::string>> read_array(std::string previous_line);

    // read the next array item from buffer or network, it's sort of the similar with read reply, but only return string for simplicity
    boost::asio::awaitable<std::string> read_array_item();

  public:
    explicit RESPReader(boost::asio::ip::tcp::socket& socket) : socket(socket)
    {
    }

    boost::asio::awaitable<std::tuple<std::variant<std::string, std::vector<std::string>, std::int64_t, std::tuple<std::string, std::string>>, ReplyType>> read_request();
};