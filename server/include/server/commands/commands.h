#pragma once

#include <common/protocol/writer.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

extern std::unordered_map<std::string, std::function<boost::asio::awaitable<void>(const std::vector<std::string>&, reddish::common::protocol::RESPWriter&)>> command_handler;
