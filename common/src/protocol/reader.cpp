#include <protocol/reader.h>
#include <boost/asio.hpp>
#include <cctype>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

boost::asio::awaitable<std::string> RESPReader::read_line()
{
    auto buf = boost::asio::dynamic_buffer(data);
    auto n = co_await boost::asio::async_read_until(socket, buf, "\r\n", boost::asio::use_awaitable);
    if (n == 0)
    {
        throw std::logic_error("failed to read any bytes from network");
    }
    std::string line = data.substr(0, n);
    buf.consume(n);
    co_return line;
}

boost::asio::awaitable<std::string> RESPReader::read_exact(std::int32_t size)
{
    auto buf = boost::asio::dynamic_buffer(data);
    auto left_bytes_amount = size - static_cast<std::int32_t>(data.length());
    if (left_bytes_amount > 0)
    {
        std::int32_t n = co_await boost::asio::async_read(socket, buf, boost::asio::transfer_exactly(left_bytes_amount), boost::asio::use_awaitable);
        if (n != left_bytes_amount)
        {
            throw std::logic_error("failed to read desired amount of bytes from network");
        }
    }
    std::string line = data.substr(0, size);
    buf.consume(size);
    co_return line;
}

boost::asio::awaitable<std::string> RESPReader::read_array_item()
{
    auto line = co_await this->read_line();
    switch (static_cast<ReplyType>(line[0]))
    {
    case ReplyType::SimpleString:
        co_return co_await this->read_simple_string(line);
    case ReplyType::Integer:
        co_return std::to_string(co_await this->read_integer(line));
    case ReplyType::BulkString:
        co_return co_await this->read_bulk_string(line);
    default:
        throw std::logic_error("unknown array item type");
    }
}

boost::asio::awaitable<std::vector<std::string>> RESPReader::read_array(std::string previous_line)
{
    auto length = std::stoi(previous_line.substr(1, previous_line.length() - 2));
    std::vector<std::string> result;
    result.reserve(length);
    for (auto i = 0; i < length; i++)
    {
        result.push_back(co_await this->read_array_item());
    }
    co_return result;
}

boost::asio::awaitable<std::string> RESPReader::read_simple_string(std::string previous_line)
{
    co_return previous_line.substr(1, previous_line.length() - 2);
}

boost::asio::awaitable<std::string> RESPReader::read_bulk_string(std::string previous_line)
{
    auto length = std::stoi(previous_line.substr(1, previous_line.length() - 2));
    auto line = co_await this->read_exact(length + 2);
    co_return line.substr(0, length);
}

boost::asio::awaitable<std::tuple<std::string, std::string>> RESPReader::read_error(std::string previous_line)
{
    auto idx = previous_line.find_first_of(' ');
    co_return std::tuple{previous_line.substr(1, idx), previous_line.substr(idx + 1, previous_line.length() - 2)};
}

boost::asio::awaitable<std::int64_t> RESPReader::read_integer(std::string previous_line)
{
    bool is_negetive = previous_line[1] == '-';
    if (std::isdigit(previous_line[1]))
    {
        co_return std::stoll(previous_line.substr(1, previous_line.length() - 2));
    }
    else
    {
        co_return (is_negetive ? -1 : 1) * std::stoll(previous_line.substr(2, previous_line.length() - 2));
    }
}

boost::asio::awaitable<
    std::tuple<std::variant<std::string, std::vector<std::string>, std::int64_t, std::tuple<std::string, std::string>>,
               RESPReader::ReplyType>>
RESPReader::read_request()
{
    auto line = co_await this->read_line();
    switch (static_cast<ReplyType>(line[0]))
    {
    case ReplyType::SimpleString:
        co_return std::tuple{co_await this->read_simple_string(line), ReplyType::SimpleString};
    case ReplyType::Error:
        co_return std::tuple{co_await this->read_error(line), ReplyType::Error};
    case ReplyType::Integer:
        co_return std::tuple{co_await this->read_integer(line), ReplyType::Integer};
    case ReplyType::BulkString:
        co_return std::tuple{co_await this->read_bulk_string(line), ReplyType::BulkString};
    case ReplyType::Arrays:
        co_return std::tuple{co_await this->read_array(line), ReplyType::Arrays};
    default:
        throw std::logic_error("unknown request type");
    }
}
