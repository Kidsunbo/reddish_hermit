#include <boost/asio/executor.hpp>
#include <boost/asio/this_coro.hpp>
#include <server/kiedis/kiedis.h>
#include <server/kiedis/connection_handler.h>
#include <common/network/connection.h>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

std::shared_ptr<KiedisServer> KiedisServer::instance_ptr = nullptr;

boost::asio::awaitable<void> KiedisServer::run()
{
    auto ctx = co_await boost::asio::this_coro::executor;
    boost::asio::ip::tcp::acceptor acceptor(ctx, {boost::asio::ip::tcp::v4(), config.port});
    while (true)
    {
        auto socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
        std::cout << "[NEW CONNECTION] address=" << socket.remote_endpoint().address() << std::endl;
        boost::asio::co_spawn(
            ctx,
            [](boost::asio::ip::tcp::socket socket) -> boost::asio::awaitable<void> {
                auto ctx = co_await boost::asio::this_coro::executor;
                reddish::common::network::Connection conn(ctx, std::move(socket));
                ConnectionHandler handler{std::move(conn)};
                co_await handler.handle_connection();
                co_return;
            }(std::move(socket)),
            boost::asio::detached);
    }
    co_return;
}

std::shared_ptr<KiedisServer> &KiedisServer::instance()
{
    if (instance_ptr == nullptr)
    {
        instance_ptr = std::make_shared<KiedisServer>(KiedisServer::private_dummy{});
    }
    return instance_ptr;
}

void KiedisServer::parse_config_from_command_line(int argc, char** argv)
{
    for (int i=0; i<argc; i++) {
        if ((std::string_view{argv[i]} == "--port" || std::string_view{argv[i]} == "-p") && i+1 < argc) {
            this->config.port = std::stoi(std::string{argv[++i]});
        }
    }
}

const KiedisServerConfig &KiedisServer::get_config()
{
    return this->config;
}
