#include <reddish/network/connection.h>

#include <iostream>

#include <boost/regex.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
using namespace boost::asio::experimental::awaitable_operators;

using namespace reddish::network;

boost::asio::awaitable<std::string> get(Connection &conn, std::string_view host)
{

    auto v = co_await conn.connect_with_host_name(host, 80);
    if (!v)
    {
        std::cout <<"#error 1 "<<  v.error() << std::endl;
        co_return "";
    }
    std::cout <<"#value 1 "<< v.value() << std::endl;

    auto ve = co_await conn.write("GET / HTTP/1.1\r\n\r\n");
    if (!ve)
    {
        std::cout <<"#error 2 "<<  ve.error().message() << std::endl;
        co_return "";
    }
    std::cout << "#value 2 "<< ve.value() << std::endl;

    std::string s;
    boost::asio::dynamic_string_buffer buf(s);
    ve = co_await conn.read_until(buf, "\r\n\r\n");
    if (!ve)
    {
        std::cout << "#error 3 " <<ve.error() << std::endl;
        co_return "";
    }
    std::cout << "#value 3 "<< ve.value() << std::endl;

    std::size_t total_size = 0;
    boost::regex re("Content-Length: ([0-9]+)");
    boost::smatch what;
    if (boost::regex_search(s, what, re))
    {
        total_size = std::stoi(what[1]);
    }
    else
    {
        std::cout << "Failed to parse Content-Length" << std::endl;
        co_return "";
    }
    buf.consume(ve.value());

    ve = co_await conn.read_exact(buf, total_size - s.length());
    if (!ve)
    {
        std::cout << "#error 4 "<< ve.error() << std::endl;
        co_return "";
    }
    std::cout <<"#value 4 "<< ve.value() << std::endl;
    buf.consume(ve.value());
    co_return s;
}

boost::asio::awaitable<void> co_main()
{
    auto ctx = co_await boost::asio::this_coro::executor;

    Connection conn(ctx, 5);
    Connection conn_without_timeout(ctx);

    std::vector<std::string> vec = {
        "github.com",
        "www.baidu.com",
        "www.google.com",
        "www.youtube.com",
        "www.163.com",

    };
    for(auto& item : vec){
        auto s = co_await get(conn, item);
        std::cout<<s<<std::endl;
    }

    for(auto& item : vec){
        auto s = co_await get(conn_without_timeout, item);
        std::cout<<s<<std::endl;
    }
}

int main()
{
    boost::asio::io_context ctx;
    boost::asio::co_spawn(ctx, co_main(), boost::asio::detached);
    ctx.run();
    return 0;
}