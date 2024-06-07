#include <boost/asio.hpp>
#include <cctype>
#include <common/protocol/reader.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace reddish::common::protocol {

    boost::asio::awaitable<std::string> RESPReader::read_line()
    {
        auto content = co_await conn.read_until("\r\n");
        if (!content) {
            throw std::logic_error("failed to read any bytes from network");
        }
        co_return content.value();
    }

    boost::asio::awaitable<std::string> RESPReader::read_exact(std::int32_t size)
    {
        
        auto result = co_await conn.read_exact(size);
        if (!result) {
            throw std::logic_error("failed to read desired amount of bytes from network");
        }
        co_return result.value();
    }

    boost::asio::awaitable<std::string> RESPReader::read_array_item()
    {
        auto line = co_await this->read_line();
        switch (static_cast<RESPDataType>(line[0])) {
        case RESPDataType::SimpleString:
            co_return co_await this->read_simple_string(line);
        case RESPDataType::Integer:
            co_return std::to_string(co_await this->read_integer(line));
        case RESPDataType::BulkString:
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
        for (auto i = 0; i < length; i++) {
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
        co_return std::tuple { previous_line.substr(1, idx), previous_line.substr(idx + 1, previous_line.length() - 2) };
    }

    boost::asio::awaitable<std::int64_t> RESPReader::read_integer(std::string previous_line)
    {
        bool is_negetive = previous_line[1] == '-';
        if (std::isdigit(previous_line[1])) {
            co_return std::stoll(previous_line.substr(1, previous_line.length() - 2));
        } else {
            co_return (is_negetive ? -1 : 1) * std::stoll(previous_line.substr(2, previous_line.length() - 2));
        }
    }

    boost::asio::awaitable<
        std::tuple<std::variant<std::string, std::vector<std::string>, std::int64_t, std::tuple<std::string, std::string>>,
            RESPDataType>>
    RESPReader::read_request()
    {
        auto line = co_await this->read_line();
        switch (static_cast<RESPDataType>(line[0])) {
        case RESPDataType::SimpleString:
            co_return std::tuple { co_await this->read_simple_string(line), RESPDataType::SimpleString };
        case RESPDataType::Error:
            co_return std::tuple { co_await this->read_error(line), RESPDataType::Error };
        case RESPDataType::Integer:
            co_return std::tuple { co_await this->read_integer(line), RESPDataType::Integer };
        case RESPDataType::BulkString:
            co_return std::tuple { co_await this->read_bulk_string(line), RESPDataType::BulkString };
        case RESPDataType::Arrays:
            co_return std::tuple { co_await this->read_array(line), RESPDataType::Arrays };
        default:
            throw std::logic_error("unknown request type");
        }
    }

}
