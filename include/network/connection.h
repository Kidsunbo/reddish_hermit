#ifndef REDDISH_HERMIT_NETWORK_CONNECTION_H
#define REDDISH_HERMIT_NETWORK_CONNECTION_H

#include <boost/asio.hpp>
#include <tuple>

namespace reddish::network
{
    class Connection{
        boost::asio::any_io_executor& ctx;
        boost::asio::ip::tcp::socket sock;
        int timeout;

        template<typename T>
        using Result = boost::asio::awaitable<std::tuple<T, boost::system::error_code>>;
        using Buffer = boost::asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>;
        public:
        explicit Connection(boost::asio::any_io_executor& ctx, int timeout=0);

        Result<boost::asio::ip::tcp::endpoint> connect_with_host_name(std::string_view host, unsigned short port);
        Result<boost::asio::ip::tcp::endpoint> connect_with_ip(const std::string& ip, unsigned short port);
        Result<std::size_t> write(std::string_view value);
        Result<std::size_t> read_until(Buffer& buf, std::string_view sep);
        Result<std::size_t> read_exact(Buffer& buf, std::int64_t size);
        bool is_connected();
        boost::system::error_code close();
    };
} // namespace reddish::network


#endif