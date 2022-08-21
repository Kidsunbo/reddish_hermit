#include "network/connection.h"

#include <utils/timer.h>
#include <utils/default.h>

#include <boost/asio/experimental/awaitable_operators.hpp>

namespace net = boost::asio;
using namespace boost::asio::experimental::awaitable_operators;

namespace reddish::network
{
    Connection::Connection(boost::asio::any_io_executor &ctx, int timeout) : ctx(ctx), sock(ctx), timeout(timeout) {}

    Connection::Result<boost::asio::ip::tcp::endpoint> Connection::connect_with_host_name(std::string_view host, unsigned short port)
    {
        boost::system::error_code ec;

        net::ip::tcp::resolver resolver(ctx);
        boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> address;
        if(timeout > 0){
            auto result = co_await (resolver.async_resolve(host, std::to_string(port), net::redirect_error(net::use_awaitable, ec))|| utils::timeout(ctx, timeout));
            if(result.index()==1){
                co_return std::make_tuple(utils::empty_ep,utils::empty_ec);
            }
            if(ec){
                co_return std::make_tuple(utils::empty_ep, ec);
            }
            address = std::get<0>(result);
        }else{
            address = co_await resolver.async_resolve(host, std::to_string(port), net::redirect_error(net::use_awaitable, ec));
            if(ec){
                co_return std::make_tuple(utils::empty_ep, ec);
            }
        }
        
        if (timeout > 0)
        {
            auto v = co_await (net::async_connect(sock, address, net::redirect_error(net::use_awaitable, ec)) || utils::timeout(ctx, timeout));
            if(v.index() == 1){
                co_return std::make_tuple(utils::empty_ep, std::get<1>(v));
            }
            if(ec){
                co_return std::make_tuple(utils::empty_ep, ec);
            }
            co_return std::make_tuple(std::get<0>(v), utils::empty_ec);
        }
        else
        {
            auto v = co_await net::async_connect(sock, address, net::redirect_error(net::use_awaitable, ec));
            if (ec)
            {
                co_return std::make_tuple(utils::empty_ep, ec);
            }
            co_return std::make_tuple(v, utils::empty_ec);
        }
    }

    Connection::Result<boost::asio::ip::tcp::endpoint> Connection::connect_with_ip(const std::string &ip, unsigned short port)
    {
        boost::system::error_code ec;

        net::ip::address ip_address = net::ip::address::from_string(ip, ec);
        if (ec)
        {
            co_return std::make_tuple(utils::empty_ep, ec);
        }
        net::ip::tcp::endpoint ep(ip_address, port);
        if (timeout > 0)
        {
            auto v = co_await (sock.async_connect(ep, net::redirect_error(net::use_awaitable, ec)) || utils::timeout(ctx, timeout));
            if(v.index() == 1){
                co_return std::make_tuple(utils::empty_ep, std::get<1>(v));
            }
            if(ec){
                co_return std::make_tuple(utils::empty_ep, ec);
            }
            co_return std::make_tuple(ep, utils::empty_ec);
        }
        else
        {
            co_await sock.async_connect(ep, net::redirect_error(net::use_awaitable, ec));
            if (ec)
            {
                co_return std::make_tuple(utils::empty_ep, ec);
            }
            co_return std::make_tuple(ep, utils::empty_ec);
        }
    }

    Connection::Result<std::size_t> Connection::write(std::string_view value){
        boost::system::error_code ec;

        if(timeout > 0){
            auto v = co_await (net::async_write(sock, net::buffer(value), net::redirect_error(net::use_awaitable, ec))||utils::timeout(ctx, timeout));
            if(v.index() == 1){
                co_return std::make_tuple(0, std::get<1>(v));
            }
            co_return std::make_tuple(std::get<0>(v), ec);
        }else{
            auto n = co_await net::async_write(sock, net::buffer(value), net::redirect_error(net::use_awaitable, ec));
            co_return std::make_tuple(n, ec);
        }

    }


    Connection::Result<std::size_t> Connection::read_until(Buffer& buf, std::string_view sep){
        boost::system::error_code ec;
        if(timeout > 0){
            auto v = co_await (net::async_read_until(sock, buf, sep, net::redirect_error(net::use_awaitable, ec)) || utils::timeout(ctx, timeout));
            if(v.index() == 1){
                co_return std::make_tuple(0, std::get<1>(v));
            }
            co_return std::make_tuple(std::get<0>(v), ec);
        }else{
            auto n = co_await net::async_read_until(sock, buf, sep, net::redirect_error(net::use_awaitable, ec));
            if(ec){
                co_return std::make_tuple(0, ec);
            }
            co_return std::make_tuple(n, utils::empty_ec);
        }
    }

    Connection::Result<std::size_t> Connection::read_exact(Buffer& buf, std::int64_t size){
        if(size == 0){
            co_return std::make_tuple(0, utils::empty_ec);
        }
        else if(size <0){
            co_return std::make_tuple(0, boost::system::errc::make_error_code(boost::system::errc::invalid_argument));
        }

        boost::system::error_code ec;
        if(timeout > 0){
            auto v = co_await (net::async_read(sock, buf, net::transfer_exactly(size), net::redirect_error(net::use_awaitable, ec)) || utils::timeout(ctx, timeout));
            if(v.index() == 1){
                co_return std::make_tuple(0, std::get<1>(v));
            }
            co_return std::make_tuple(std::get<0>(v), ec);
        }else{
            auto n = co_await net::async_read(sock, buf, net::transfer_exactly(size), net::redirect_error(net::use_awaitable, ec));
            if(ec){
                co_return std::make_tuple(0, ec);
            }
            co_return std::make_tuple(n, utils::empty_ec);
        }
    }

    boost::system::error_code Connection::close(){
        boost::system::error_code ec;
        sock.close(ec);
        return ec;
    }

} // namespace reddish::network
