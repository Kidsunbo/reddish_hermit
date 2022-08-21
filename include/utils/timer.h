#ifndef REDDISH_HERMIT_UTILS_TIMER_H
#define REDDISH_HERMIT_UTILS_TIMER_H

#include <boost/asio.hpp>

namespace reddish::utils
{
    boost::asio::awaitable<boost::system::error_code> timeout(boost::asio::any_io_executor& ctx, int sec);
} // namespace reddish::utils


#endif