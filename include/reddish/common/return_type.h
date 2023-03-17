#ifndef REDDISH_HERMIT_COMMON_RETURN_TYPE_H
#define REDDISH_HERMIT_COMMON_RETURN_TYPE_H

#include <boost/asio.hpp>
#include <boost/outcome.hpp>

namespace reddish
{
    template<typename T>
    using AsyncResult = boost::asio::awaitable<boost::outcome_v2::result<T>>;

    using Buffer = boost::asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>;
    
    template<typename T>
    using SyncResult = boost::outcome_v2::result<T>;

} // namespace reddish



#endif