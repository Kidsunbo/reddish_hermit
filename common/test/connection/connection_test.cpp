#include <common/network/connection.h>
#include <iostream>

#include <boost/regex.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
using namespace boost::asio::experimental::awaitable_operators;

using namespace reddish::common::network;

boost::asio::awaitable<std::string> get(Connection &conn, std::string_view host)
{

    auto v = co_await conn.connect_with_host_name(host, 80);
    if (!v)
    {
        std::cout <<"failed to parse host: "<<  v.error().message() << std::endl;
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

    auto content = co_await conn.read_until("\r\n\r\n");
    if (!content)
    {
        std::cout << "Failed to read response " <<content.error() << std::endl;
        co_return "";
    }
    std::cout << "Read response with "<< content.value().length() <<" bytes" << std::endl;

    std::size_t total_size = 0;
    boost::regex re("Content-Length: ([0-9]+)");
    boost::smatch what;
    if (boost::regex_search(content.value(), what, re))
    {
        total_size = std::stoi(what[1]);
    }
    else
    {
        std::cout << "Failed to parse Content-Length" << std::endl;
        co_return "";
    }
    std::string result = content.value();

    content = co_await conn.read_exact(total_size);
    if (!content)
    {
        std::cout << "Failed to read body "<< content.error() << std::endl;
        co_return "";
    }
    std::cout <<"Read body with "<< content.value().length() <<" bytes" << std::endl;
    result.append(content.value());
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
        std::ignore = conn.close();
        std::cout<<item<<std::endl<<s<<std::endl;
    }
}

boost::asio::awaitable<void> co_main(){
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
    auto content = co_await conn.read_until("\r\n");
    if(!content){
        std::cout<<content.error().message()<<std::endl;
        co_return;
    }
    std::cout<<"Return Value "<<content.value().length()<<"\n"<<content.value()<<std::endl;
}

int main()
{
    boost::asio::io_context ctx;
    boost::asio::co_spawn(ctx, co_main_http(), boost::asio::detached);
    // boost::asio::co_spawn(ctx, co_main(), boost::asio::detached);
    ctx.run();
    return 0;
}