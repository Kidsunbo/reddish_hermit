#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <common/network/connection.h>
#include <gtest/gtest.h>
#include <protocol/loopback_fixture.h>

#include <cstdint>
#include <string>
#include <utility>

namespace reddish::common::network {
namespace {

    using namespace boost::asio::experimental::awaitable_operators;
    using reddish::common::protocol::test::LoopbackPair;

    struct Captured {
        std::string error;
        bool ok { false };
    };

    template <typename Result>
    bool fail(Captured& out, const Result& result, const char* message)
    {
        if (result) {
            return false;
        }
        out.error = std::string(message) + result.error().message();
        return true;
    }

    boost::asio::awaitable<void> write_and_read_back(LoopbackPair& pair, Captured& out)
    {
        auto written = co_await (*pair.client).write("ping");
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto echoed = co_await (*pair.server).read_exact(4);
        if (fail(out, echoed, "read_exact failed: ")) {
            co_return;
        }
        if (echoed.value() != "ping") {
            out.error = "echo mismatch";
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> read_until_multiple_messages(LoopbackPair& pair, Captured& out)
    {
        auto written = co_await (*pair.client).write("line1\r\nline2\r\n");
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto first = co_await (*pair.server).read_until("\r\n");
        if (fail(out, first, "read_until failed: ")) {
            co_return;
        }
        if (first.value() != "line1\r\n") {
            out.error = "first line mismatch";
            co_return;
        }
        auto second = co_await (*pair.server).read_until("\r\n");
        if (fail(out, second, "read_until failed: ")) {
            co_return;
        }
        if (second.value() != "line2\r\n") {
            out.error = "second line mismatch";
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> read_exact_across_chunks(LoopbackPair& pair, Captured& out)
    {
        auto written = co_await (*pair.client).write("abcdef");
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto first = co_await (*pair.server).read_exact(2);
        if (fail(out, first, "read_exact failed: ")) {
            co_return;
        }
        if (first.value() != "ab") {
            out.error = "first chunk mismatch";
            co_return;
        }
        auto rest = co_await (*pair.server).read_exact(4);
        if (fail(out, rest, "read_exact failed: ")) {
            co_return;
        }
        if (rest.value() != "cdef") {
            out.error = "rest chunk mismatch";
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> read_until_then_exact(LoopbackPair& pair, Captured& out)
    {
        auto written = co_await (*pair.client).write("$3\r\nfoo\r\n");
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto header = co_await (*pair.server).read_until("\r\n");
        if (fail(out, header, "read_until failed: ")) {
            co_return;
        }
        if (header.value() != "$3\r\n") {
            out.error = "header mismatch";
            co_return;
        }
        auto payload = co_await (*pair.server).read_exact(3);
        if (fail(out, payload, "read_exact failed: ")) {
            co_return;
        }
        if (payload.value() != "foo") {
            out.error = "payload mismatch";
            co_return;
        }
        auto crlf = co_await (*pair.server).read_exact(2);
        if (fail(out, crlf, "read_exact failed: ")) {
            co_return;
        }
        if (crlf.value() != "\r\n") {
            out.error = "trailer mismatch";
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> peer_close_yields_eof(LoopbackPair& pair, Captured& out)
    {
        boost::system::error_code close_ec = (*pair.client).close();
        if (close_ec) {
            out.error = "close failed: " + close_ec.message();
            co_return;
        }
        auto result = co_await (*pair.server).read_until("\r\n");
        if (result) {
            out.error = "expected EOF got data";
            co_return;
        }
        if (result.error() != boost::asio::error::eof
            && result.error() != boost::asio::error::connection_reset) {
            out.error = "unexpected error: " + result.error().message();
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> write_empty(LoopbackPair& pair, Captured& out)
    {
        auto written = co_await (*pair.client).write("");
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        if (written.value() != 0u) {
            out.error = "expected zero bytes written";
            co_return;
        }
        out.ok = true;
    }

    void run_io(LoopbackPair& pair, boost::asio::awaitable<void> coro)
    {
        boost::asio::co_spawn(pair.ctx, std::move(coro), boost::asio::detached);
        pair.ctx.run();
    }

    TEST(Connection, WriteAndReadBack)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, write_and_read_back(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Connection, ReadUntilMultipleMessages)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, read_until_multiple_messages(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Connection, ReadExactAcrossChunks)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, read_exact_across_chunks(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Connection, ReadUntilThenExact)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, read_until_then_exact(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Connection, PeerCloseYieldsEof)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, peer_close_yields_eof(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Connection, WriteEmpty)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, write_empty(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

}
}