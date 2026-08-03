#include <server/commands/commands.h>

std::unordered_map<std::string, std::function<boost::asio::awaitable<void>(const std::vector<std::string>&, reddish::common::protocol::RESPWriter&)>> command_handler;
