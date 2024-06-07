#pragma once

#include <protocol/writer.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

extern std::unordered_map<std::string, std::function<boost::asio::awaitable<void>(const std::vector<std::string>&, RESPWriter&)>> command_handler;