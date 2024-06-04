#include <kiedis/kiedis.h>
#include <boost/asio.hpp>
#include <csignal>
#include <exception>
#include <iostream>
#include <memory>

using namespace std::string_view_literals;

boost::asio::awaitable<void> co_main(int argc, char **argv)
{
    auto server = KiedisServer::instance();
    server->parse_config_from_command_line(argc, argv);
    co_await server->run();
}

int main(int argc, char **argv)
{
    try
    {
        boost::asio::io_context ctx;
        boost::asio::signal_set signals(ctx, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { ctx.stop(); });
        boost::asio::co_spawn(ctx, co_main(argc, argv), boost::asio::detached);
        ctx.run();
    }
    catch (std::exception &e)
    {
        std::cout << "[EXCEPTION] " << e.what() << std::endl;
    }
    return 0;
}
