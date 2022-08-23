#include "utils/timer.h"


namespace reddish::utils
{
    // boost::asio::awaitable<boost::system::error_code> timeout(boost::asio::any_io_executor& ctx, int sec){
    //     boost::asio::steady_timer timer(ctx);
    //     timer.expires_after(std::chrono::seconds(sec));
    //     boost::system::error_code ec;
    //     co_await timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    //     co_return ec;
    // }
} // namespace reddish::utils
