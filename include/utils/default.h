#ifndef REDDISH_HERMIT_UTILS_DEFAULT_H
#define REDDISH_HERMIT_UTILS_DEFAULT_H

#include <boost/system.hpp>
#include <boost/asio.hpp>

namespace reddish::utils
{
    extern boost::system::error_code empty_ec;
    extern boost::asio::ip::tcp::endpoint empty_ep;
} // namespace reddish::utils


#endif