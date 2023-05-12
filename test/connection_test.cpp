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
        std::cout <<"failed to parse host: "<<  v.error() << std::endl;
        co_return "";
    }
    std::cout <<"IP address: "<< v.value() << std::endl;

    auto ve = co_await conn.write("GET / HTTP/1.1\r\n\r\n");
    if (!ve)
    {
        std::cout <<"Failed to request: "<<  ve.error().message() << std::endl;
        co_return "";
    }
    std::cout << "Write request with "<< ve.value()<<" bytes" << std::endl;

    std::string s;
    boost::asio::dynamic_string_buffer buf(s);
    ve = co_await conn.read_until(buf, "\r\n\r\n");
    if (!ve)
    {
        std::cout << "Failed to read response " <<ve.error() << std::endl;
        co_return "";
    }
    std::cout << "Read response with "<< ve.value() <<" bytes" << std::endl;

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
    std::string result = s.substr(0, ve.value());
    buf.consume(ve.value());

    ve = co_await conn.read_exact(buf, total_size - s.length());
    if (!ve)
    {
        std::cout << "Failed to read body "<< ve.error() << std::endl;
        co_return "";
    }
    std::cout <<"Read body with "<< ve.value() <<" bytes" << std::endl;
    result.append(s);
    co_return result;
}

boost::asio::awaitable<void> co_main_http()
{
    auto any_ctx = co_await boost::asio::this_coro::executor;
    Connection conn(any_ctx, 5);
    
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
}

boost::asio::awaitable<void> co_main_redis(){
    auto ctx = co_await boost::asio::this_coro::executor;
    Connection conn(ctx, 5);
    if(auto result = co_await conn.connect_with_host_name("host.docker.internal", 6379); result.has_error()){
        std::cerr<<"connect: "<<result.error()<<std::endl;
        co_return;
    }
    if(auto result = co_await conn.write("*1\r\n$7\r\nCOMMAND\r\n"); result.has_error()){
        std::cerr<<"write: "<<result.error()<<std::endl;
        co_return;
    }
    std::string s;
    boost::asio::dynamic_string_buffer buf(s);
    auto n = co_await conn.read_until(buf, "\r\n");
    if(!n){
        std::cout<<n.error().message()<<std::endl;
        co_return;
    }
    std::cout<<"Return Value "<<n.value()<<"\n"<<s<<std::endl;
    buf.consume(n.value());
}

int main()
{
    boost::asio::io_context ctx;
    // boost::asio::co_spawn(ctx, co_main_http(), boost::asio::detached);
    boost::asio::co_spawn(ctx, co_main_redis(), boost::asio::detached);
    ctx.run();
    return 0;
}