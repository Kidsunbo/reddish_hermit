#include <algorithm>
#include <cctype>
#include <common/protocol/resp_data_type.h>
#include <server/kiedis/connection_handler.h>

using namespace reddish::common::protocol;

boost::asio::awaitable<void> ConnectionHandler::handle_connection()
{
    auto ctx = co_await boost::asio::this_coro::executor;
    try {
        while (true) {
            auto [data, type] = co_await reader.read_request();
            switch (type) {
            case RESPDataType::SimpleString:
            case RESPDataType::Error:
            case RESPDataType::Integer:
                co_await this->handle_unsupported_command();
                break;
            case RESPDataType::BulkString: {
                std::vector<std::string> temp_data { std::get<std::string>(data) };
                co_await this->handle_request(temp_data);
                break;
            }
            case RESPDataType::Arrays:
                co_await this->handle_request(std::get<std::vector<std::string>>(data));
                break;
            }
        }
    } catch (std::exception& e) {
        // std::cout << "[EXCEPTION] " << e.what() << std::endl;
    }
    co_return;
}

boost::asio::awaitable<void> ConnectionHandler::handle_unsupported_command()
{
    co_await writer.write_error("Err", "Unsupported Command");
    co_return;
}

boost::asio::awaitable<void> ConnectionHandler::handle_request(const std::vector<std::string>& value)
{
    if (value.size() == 0) {
        throw std::logic_error("array length is zero");
    }
    std::string cmd = value[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](auto a) { return std::toupper(a); });

    co_await writer.write_error("Err", "Server doesn't support this command");
    co_return;
}
