#include <algorithm>
#include <cctype>
#include <common/protocol/resp_value.h>
#include <server/commands/commands.h>
#include <server/kiedis/connection_handler.h>

using namespace reddish::common::protocol;

boost::asio::awaitable<void> ConnectionHandler::handle_connection()
{
    auto ctx = co_await boost::asio::this_coro::executor;
    try {
        while (true) {
            auto result = co_await reader.read();
            if (!result) {
                if (result.error() == boost::system::errc::protocol_error) {
                    co_await writer.write_error("ERR", "Protocol error...");
                    boost::system::error_code close_ec = conn.close();
                    (void)close_ec;
                }
                break;
            }

            const RESPValue& value = result.value();
            switch (value.type) {
            case RESPValue::Type::Array: {
                std::vector<std::string> args;
                args.reserve(value.array_value().size());
                bool valid = true;
                for (const RESPValue& item : value.array_value()) {
                    if (item.type != RESPValue::Type::BulkString) {
                        valid = false;
                        break;
                    }
                    args.push_back(item.string_value());
                }
                if (!valid) {
                    co_await writer.write_error("ERR", "unknown request type");
                    break;
                }
                co_await this->handle_request(args);
                break;
            }
            case RESPValue::Type::BulkString: {
                std::vector<std::string> args;
                args.push_back(value.string_value());
                co_await this->handle_request(args);
                break;
            }
            default:
                co_await writer.write_error("ERR", "unknown request type");
                break;
            }
        }
    } catch (std::exception& e) {
        std::cout << "[EXCEPTION] " << e.what() << std::endl;
    }
    co_return;
}

boost::asio::awaitable<void> ConnectionHandler::handle_unsupported_command()
{
    co_await writer.write_error("ERR", "unknown request type");
    co_return;
}

boost::asio::awaitable<void> ConnectionHandler::handle_request(const std::vector<std::string>& value)
{
    if (value.empty()) {
        co_await writer.write_error("ERR", "unknown request type");
        co_return;
    }
    std::string cmd = value[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](auto a) { return std::toupper(a); });

    auto it = command_handler.find(cmd);
    if (it == command_handler.end()) {
        co_await writer.write_error("ERR", "unknown command '" + cmd + "'");
        co_return;
    }
    co_await it->second(value, writer);
    co_return;
}
