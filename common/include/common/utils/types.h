#ifndef REDDISH_HERMIT_COMMON_COMMAND_COMMAND_H
#define REDDISH_HERMIT_COMMON_COMMAND_COMMAND_H

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/outcome.hpp>
#include <memory>
#include <string>
#include <variant>

namespace reddish::common::utils
{
    template<typename T>
    using AsyncResult = boost::asio::awaitable<boost::outcome_v2::result<T>>;

    template<typename T>
    using SyncResult = boost::outcome_v2::result<T>;

    using Buffer = boost::asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>;

}

#endif