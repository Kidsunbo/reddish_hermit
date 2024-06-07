#include "common/network/connection.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/system/detail/errc.hpp>
#include <cstdint>

namespace net = boost::asio;
using namespace boost::asio::experimental::awaitable_operators;

namespace reddish::common::network {
    Connection::Connection(boost::asio::any_io_executor ctx, int timeout)
        : ctx(ctx)
        , sock(ctx)
        , timeout(timeout)
        , timer(ctx)
        , data("")
        , buf(boost::asio::dynamic_string_buffer(data))
    {
    }
    Connection::Connection(boost::asio::io_context& ctx, int timeout)
        : Connection(ctx.get_executor(), timeout)
    {
    }
    Connection::Connection(boost::asio::io_context&& ctx, int timeout)
        : Connection(ctx.get_executor(), timeout)
    {
    }

    Connection::Connection(boost::asio::any_io_executor ctx, boost::asio::ip::tcp::socket&& sock, int timeout)
        : ctx(ctx)
        , sock(std::move(sock))
        , timeout(timeout)
        , timer(ctx)
        , data("")
        , buf(boost::asio::dynamic_string_buffer(data))
    {
    }

    AsyncResult<boost::asio::ip::tcp::endpoint> Connection::connect_with_host_name(std::string_view host, unsigned short port)
    {
        if (sock.is_open()) {
            co_return boost::system::errc::operation_not_permitted;
        }

        boost::system::error_code ec;
        net::ip::tcp::resolver resolver(ctx);
        boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> address;
        if (timeout > 0) {
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (resolver.async_resolve(host, std::to_string(port), net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if (std::holds_alternative<std::monostate>(result)) {
                co_return boost::system::errc::timed_out;
            }
            if (ec) {
                co_return ec;
            }
            address = std::get<0>(result);
        } else {
            address = co_await resolver.async_resolve(host, std::to_string(port), net::redirect_error(net::use_awaitable, ec));
            if (ec) {
                co_return ec;
            }
        }

        if (timeout > 0) {
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (net::async_connect(sock, address, net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if (std::holds_alternative<std::monostate>(result)) {
                co_return boost::system::errc::timed_out;
            }
            if (ec) {
                co_return ec;
            }
            co_return std::get<0>(result);
        } else {
            auto result = co_await net::async_connect(sock, address, net::redirect_error(net::use_awaitable, ec));
            if (ec) {
                co_return ec;
            }
            co_return result;
        }
    }

    AsyncResult<boost::asio::ip::tcp::endpoint> Connection::connect_with_ip(const std::string& ip, unsigned short port)
    {
        if (sock.is_open()) {
            co_return boost::system::errc::operation_not_permitted;
        }

        boost::system::error_code ec;

        net::ip::address ip_address = net::ip::address::from_string(ip, ec);
        if (ec) {
            co_return ec;
        }
        net::ip::tcp::endpoint ep(ip_address, port);
        if (timeout > 0) {
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (sock.async_connect(ep, net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if (result.index() == 1) {
                co_return boost::system::errc::timed_out;
            }
            if (ec) {
                co_return ec;
            }
            co_return ep;
        } else {
            co_await sock.async_connect(ep, net::redirect_error(net::use_awaitable, ec));
            if (ec) {
                co_return ec;
            }
            co_return ep;
        }
    }

    AsyncResult<std::size_t> Connection::write(std::string_view value)
    {
        if (!sock.is_open()) {
            co_return boost::system::errc::operation_not_permitted;
        }

        boost::system::error_code ec;

        if (timeout > 0) {
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (net::async_write(sock, net::buffer(value), net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if (std::holds_alternative<std::monostate>(result)) {
                co_return boost::system::errc::timed_out;
            }
            if (ec) {
                co_return ec;
            }
            co_return std::get<0>(result);
        } else {
            auto n = co_await net::async_write(sock, net::buffer(value), net::redirect_error(net::use_awaitable, ec));
            if (ec) {
                co_return ec;
            }
            co_return n;
        }
    }

    AsyncResult<std::string> Connection::read_until(std::string_view sep)
    {
        if (!sock.is_open()) {
            co_return boost::system::errc::operation_not_permitted;
        }

        boost::system::error_code ec;
        std::uint64_t size;
        if (timeout > 0) {
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (net::async_read_until(sock, buf, sep, net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if (std::holds_alternative<std::monostate>(result)) {
                co_return boost::system::errc::timed_out;
            }
            if (ec) {
                co_return ec;
            }
            size = std::get<0>(result);
        } else {
            auto n = co_await net::async_read_until(sock, buf, sep, net::redirect_error(net::use_awaitable, ec));
            if (ec) {
                co_return ec;
            }
            size = n;
        }
        std::string result = data.substr(0, size);
        buf.consume(size);
        co_return result;
    }

    AsyncResult<std::string> Connection::read_exact(std::uint64_t size)
    {
        if (!sock.is_open()) {
            co_return boost::system::errc::operation_not_permitted;
        }

        if (size == 0) {
            co_return "";
        }

        if (data.length() < size) {
            auto left_size = size - data.length();
            boost::system::error_code ec;
            if (timeout > 0) {
                timer.expires_after(std::chrono::seconds(timeout));
                auto result = co_await (net::async_read(sock, buf, net::transfer_exactly(left_size), net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
                if (std::holds_alternative<std::monostate>(result)) {
                    co_return boost::system::errc::timed_out;
                }
                if (ec) {
                    co_return ec;
                }
                if(std::get<0>(result) != left_size){
                    co_return boost::system::errc::protocol_error;
                }
            } else {
                auto n = co_await net::async_read(sock, buf, net::transfer_exactly(left_size), net::redirect_error(net::use_awaitable, ec));
                if (ec) {
                    co_return ec;
                }
                if(n != left_size) {
                    co_return boost::system::errc::protocol_error;
                }
            }
        }
        std::string result = data.substr(0, size);
        buf.consume(size);
        co_return result;
    }

    boost::system::error_code Connection::close()
    {
        boost::system::error_code ec;
        std::ignore = sock.shutdown(net::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            return ec;
        }
        std::ignore = sock.close(ec);
        return ec;
    }

    Connection::~Connection() noexcept
    {
        if (sock.is_open()) {
            std::ignore = close();
        }
    }

    void Connection::reset_buffer()
    {
        data = "";
    }
}
