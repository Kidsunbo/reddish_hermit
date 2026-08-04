#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <common/network/connection.h>

#include <memory>
#include <stdexcept>
#include <utility>

namespace reddish::common::protocol::test {

    // A localhost socket pair sharing one io_context. `client` and `server`
    // are already connected to each other; the owner must ensure any
    // coroutines are spawned on the same io_context.
    class LoopbackPair {
    public:
        LoopbackPair()
            : ctx()
        {
            using boost::asio::ip::tcp;

            tcp::acceptor acceptor(ctx, tcp::endpoint(tcp::v4(), 0));
            const tcp::endpoint endpoint = acceptor.local_endpoint();

            tcp::socket client_sock(ctx);
            boost::system::error_code connect_ec;
            client_sock.connect(endpoint, connect_ec);
            if (connect_ec) {
                throw std::runtime_error("loopback connect failed: " + connect_ec.message());
            }

            boost::system::error_code accept_ec;
            tcp::socket server_sock = acceptor.accept(accept_ec);
            if (accept_ec) {
                throw std::runtime_error("loopback accept failed: " + accept_ec.message());
            }

            client = std::make_unique<network::Connection>(
                ctx.get_executor(), std::move(client_sock));
            server = std::make_unique<network::Connection>(
                ctx.get_executor(), std::move(server_sock));
        }

        boost::asio::io_context ctx;
        std::unique_ptr<network::Connection> client;
        std::unique_ptr<network::Connection> server;
    };

}