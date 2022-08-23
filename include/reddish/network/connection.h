#ifndef REDDISH_HERMIT_NETWORK_CONNECTION_H
#define REDDISH_HERMIT_NETWORK_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/outcome.hpp>
#include <tuple>

namespace reddish::network
{
    class Connection{
        boost::asio::any_io_executor ctx;
        boost::asio::ip::tcp::socket sock;
        int timeout;
        boost::asio::steady_timer timer;

        template<typename T>
        using Result = boost::asio::awaitable<boost::outcome_v2::result<T>>;
        using Buffer = boost::asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>;
        public:
        explicit Connection(boost::asio::any_io_executor ctx, int timeout=0);
        explicit Connection(boost::asio::io_context& ctx, int timeout=0);
        explicit Connection(boost::asio::io_context&& ctx, int timeout=0);


        [[nodiscard]] Result<boost::asio::ip::tcp::endpoint> connect_with_host_name(std::string_view host, unsigned short port);
        [[nodiscard]] Result<boost::asio::ip::tcp::endpoint> connect_with_ip(const std::string& ip, unsigned short port);
        [[nodiscard]] Result<std::size_t> write(std::string_view value);
        [[nodiscard]] Result<std::size_t> read_until(Buffer& buf, std::string_view sep);
        [[nodiscard]] Result<std::size_t> read_exact(Buffer& buf, std::int64_t size);
        [[nodiscard]] bool is_connected();
        [[nodiscard]] boost::system::error_code close();

        ~Connection()noexcept;
    };
} // namespace reddish::network


#endif