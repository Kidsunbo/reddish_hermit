#include "reddish/network/connection.h"

#include <boost/asio/experimental/awaitable_operators.hpp>

namespace net = boost::asio;
using namespace boost::asio::experimental::awaitable_operators;

namespace reddish::network
{
    Connection::Connection(boost::asio::any_io_executor ctx, int timeout) : ctx(ctx), sock(ctx), timeout(timeout),timer(ctx) {}
    Connection::Connection(boost::asio::io_context& ctx, int timeout):ctx(ctx.get_executor()), sock(ctx), timeout(timeout),timer(ctx){}
    Connection::Connection(boost::asio::io_context&& ctx, int timeout):ctx(ctx.get_executor()), sock(ctx), timeout(timeout),timer(ctx){}


    Connection::Result<boost::asio::ip::tcp::endpoint> Connection::connect_with_host_name(std::string_view host, unsigned short port)
    {
        boost::system::error_code ec;
        net::ip::tcp::resolver resolver(ctx);
        boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> address;
        if(timeout > 0){
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (resolver.async_resolve(host, std::to_string(port), net::redirect_error(net::use_awaitable, ec))|| timer.async_wait(net::use_awaitable));
            if(std::holds_alternative<std::monostate>(result)){
                co_return boost::system::errc::timed_out;
            }
            if(ec){
                co_return ec;
            }
            address = std::get<0>(result);
        }else{
            address = co_await resolver.async_resolve(host, std::to_string(port), net::redirect_error(net::use_awaitable, ec));
            if(ec){
                co_return ec;
            }
        }
        
        if (timeout > 0)
        {
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (net::async_connect(sock, address, net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if(std::holds_alternative<std::monostate>(result)){
                co_return boost::system::errc::timed_out;
            }
            if(ec){
                co_return ec;
            }
            co_return std::get<0>(result);
        }
        else
        {
            auto result = co_await net::async_connect(sock, address, net::redirect_error(net::use_awaitable, ec));
            if (ec)
            {
                co_return ec;
            }
            co_return result;
        }
    }

    Connection::Result<boost::asio::ip::tcp::endpoint> Connection::connect_with_ip(const std::string &ip, unsigned short port)
    {
        boost::system::error_code ec;

        net::ip::address ip_address = net::ip::address::from_string(ip, ec);
        if (ec)
        {
            co_return ec;
        }
        net::ip::tcp::endpoint ep(ip_address, port);
        if (timeout > 0)
        {
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (sock.async_connect(ep, net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if(result.index()==1){
                co_return boost::system::errc::timed_out;
            }
            if(ec){
                co_return ec;
            }
            co_return ep;
        }
        else
        {
            co_await sock.async_connect(ep, net::redirect_error(net::use_awaitable, ec));
            if (ec)
            {
                co_return ec;
            }
            co_return ep;
        }
    }

    Connection::Result<std::size_t> Connection::write(std::string_view value){
        boost::system::error_code ec;

        if(timeout > 0){
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (net::async_write(sock, net::buffer(value), net::redirect_error(net::use_awaitable, ec))|| timer.async_wait(net::use_awaitable));
            if(std::holds_alternative<std::monostate>(result)){
                co_return boost::system::errc::timed_out;
            }
            if(ec){
                co_return ec;
            }
            co_return std::get<0>(result);
        }else{
            auto n = co_await net::async_write(sock, net::buffer(value), net::redirect_error(net::use_awaitable, ec));
            if(ec){
                co_return ec;
            }
            co_return n;
        }

    }


    Connection::Result<std::size_t> Connection::read_until(Buffer& buf, std::string_view sep){
        boost::system::error_code ec;
        if(timeout > 0){
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (net::async_read_until(sock, buf, sep, net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if(std::holds_alternative<std::monostate>(result)){
                co_return boost::system::errc::timed_out;
            }
            if(ec){
                co_return ec;
            }
            co_return std::get<0>(result);
        }else{
            auto n = co_await net::async_read_until(sock, buf, sep, net::redirect_error(net::use_awaitable, ec));
            if(ec){
                co_return ec;
            }
            co_return n;
        }
    }

    Connection::Result<std::size_t> Connection::read_exact(Buffer& buf, std::int64_t size){
        if(size == 0){
            co_return 0;
        }
        else if(size <0){
            co_return boost::system::errc::invalid_argument;
        }

        boost::system::error_code ec;
        if(timeout > 0){
            timer.expires_after(std::chrono::seconds(timeout));
            auto result = co_await (net::async_read(sock, buf, net::transfer_exactly(size), net::redirect_error(net::use_awaitable, ec)) || timer.async_wait(net::use_awaitable));
            if(std::holds_alternative<std::monostate>(result)){
                co_return boost::system::errc::timed_out;
            }
            if(ec){
                co_return ec;
            }
            co_return std::get<0>(result);
        }else{
            auto n = co_await net::async_read(sock, buf, net::transfer_exactly(size), net::redirect_error(net::use_awaitable, ec));
            if(ec){
                co_return ec;
            }
            co_return n;
        }
    }

    boost::system::error_code Connection::close(){
        boost::system::error_code ec;
        sock.shutdown(net::ip::tcp::socket::shutdown_both, ec);
        if(ec){
            return ec;
        }
        sock.close(ec);
        return ec;
    }

    Connection::~Connection()noexcept{
        if(sock.is_open()){
            static_cast<void>(close());
        }
    }

} // namespace reddish::network
