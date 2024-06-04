#include <commands/commands.h>
#include <cstdint>
#include <storage/database.h>
#include <kiedis/kiedis.h>
#include <protocol/writer.h>
#include <boost/asio.hpp>
#include <stdexcept>
#include <string>
#include <vector>

extern KiedisDB db;

void check_at_least_length(const std::vector<std::string> &value, std::uint32_t length, bool exact)
{
    if ((exact && value.size() != length) || value.size() < length)
    {
        throw std::logic_error("Command " + value[0] + " doesn't satisfied the length requirment");
    }
}

boost::asio::awaitable<void> ping(const std::vector<std::string> &value, RESPWriter &writer)
{
    check_at_least_length(value, 1, true);
    co_await writer.write_simple_string("PONG");
}

boost::asio::awaitable<void> echo(const std::vector<std::string> &value, RESPWriter &writer)
{
    check_at_least_length(value, 2, true);
    co_await writer.write_simple_string(value[1]);
}

boost::asio::awaitable<void> set(const std::vector<std::string> &value, RESPWriter &writer)
{
    if (value.size() == 3)
    {
        db.set(value[1], value[2]);
        co_await writer.write_simple_string("OK");
    }
    else if (value.size() == 5)
    {
        db.set(value[1], value[2], std::stol(value[4]));
        co_await writer.write_simple_string("OK");
    }
    else
    {
        co_await writer.write_error("Err", "the command should be of length 3 or 5");
    }
}

boost::asio::awaitable<void> get(const std::vector<std::string> &value, RESPWriter &writer)
{
    check_at_least_length(value, 2, false);
    auto result = db.get(value[1]);
    if (result.has_value())
    {
        co_await writer.write_bulk_string(result.value());
    }
    else
    {
        co_await writer.write_null_bulk_string();
    }
}

boost::asio::awaitable<void> info(const std::vector<std::string> &, RESPWriter &writer)
{
    co_await writer.write_bulk_string(std::string{"role:"} + std::string{KiedisServer::instance()->get_config().role});
}

std::unordered_map<std::string, std::function<boost::asio::awaitable<void>(const std::vector<std::string> &, RESPWriter &)>>
    command_handler{{"PING", ping}, {"ECHO", echo}, {"SET", set}, {"GET", get}, {"INFO", info}};
