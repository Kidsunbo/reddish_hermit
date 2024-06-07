#include <boost/asio.hpp>
#include <common/protocol/writer.h>
#include <stdexcept>
#include <string>

namespace reddish::common::protocol {

    boost::asio::awaitable<std::int64_t> RESPWriter::write(std::string value)
    {
        
        auto n = co_await conn.write(value);
        if (!n || n.value() == 0) {
            throw std::logic_error("failed to write anything to network");
        }
        co_return n.value();
    }

    boost::asio::awaitable<std::int64_t> RESPWriter::write_simple_string(std::string value)
    {
        co_return co_await this->write("+" + value + "\r\n");
    }

    boost::asio::awaitable<std::int64_t> RESPWriter::write_bulk_string(std::string value)
    {
        co_return co_await this->write("$" + std::to_string(value.length()) + "\r\n" + value + "\r\n");
    }

    boost::asio::awaitable<std::int64_t> RESPWriter::write_array(std::vector<std::string> value)
    {
        std::string data = "*" + std::to_string(value.size()) + "\r\n";
        for (auto item : value) {
            data += "$" + std::to_string(item.length()) + "\r\n" + item + "\r\n";
        }
        co_return co_await this->write(data);
    }

    boost::asio::awaitable<std::int64_t> RESPWriter::write_error(std::string code, std::string value)
    {
        co_return co_await this->write("-" + code + " " + value + "\r\n");
    }

    boost::asio::awaitable<std::int64_t> RESPWriter::write_integer(std::int64_t value)
    {
        co_return co_await this->write(":" + std::to_string(value) + "\r\n");
    }

    boost::asio::awaitable<std::int64_t> RESPWriter::write_null_bulk_string()
    {
        co_return co_await this->write("$-1\r\n");
    }

}