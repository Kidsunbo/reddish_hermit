#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <common/protocol/protocol.h>
#include <common/protocol/reader.h>
#include <common/protocol/resp_value.h>
#include <common/protocol/writer.h>
#include <gtest/gtest.h>
#include <protocol/loopback_fixture.h>
#include <protocol/value_equality.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace reddish::common::protocol {
namespace {

    using namespace boost::asio::experimental::awaitable_operators;
    using test::expect_value_equal;
    using test::LoopbackPair;

    struct Captured {
        std::string error;
        bool ok { false };
        RESPValue value;
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

    boost::asio::awaitable<void> write_then_read(LoopbackPair& pair, Captured& out)
    {
        RESPWriter writer(*pair.client);
        RESPReader reader(*pair.server);

        auto written = co_await writer.write(RESPValue::bulk_string("hello"));
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto value = co_await reader.read();
        if (fail(out, value, "read failed: ")) {
            co_return;
        }
        out.ok = true;
        out.value = value.value();
    }

    boost::asio::awaitable<void> write_nested_array(LoopbackPair& pair, Captured& out)
    {
        RESPWriter writer(*pair.client);
        RESPReader reader(*pair.server);

        const auto original = RESPValue::array({
            RESPValue::integer(1),
            RESPValue::bulk_string("a"),
            RESPValue::null(),
            RESPValue::array({ RESPValue::bulk_string("nested") }),
        });
        auto written = co_await writer.write(original);
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto value = co_await reader.read();
        if (fail(out, value, "read failed: ")) {
            co_return;
        }
        out.ok = true;
        out.value = value.value();
    }

    boost::asio::awaitable<void> write_error_and_integer(LoopbackPair& pair, Captured& out)
    {
        RESPWriter writer(*pair.client);
        RESPReader reader(*pair.server);

        auto written = co_await writer.write(RESPValue::error("ERR", "boom"));
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto error = co_await reader.read();
        if (fail(out, error, "read failed: ")) {
            co_return;
        }
        auto error_value = error.value();

        written = co_await writer.write(RESPValue::integer(-123));
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        auto integer = co_await reader.read();
        if (fail(out, integer, "read failed: ")) {
            co_return;
        }
        out.ok = true;
        out.value = RESPValue::array({ error_value, integer.value() });
    }

    boost::asio::awaitable<void> read_eof_when_peer_closes(LoopbackPair& pair, Captured& out)
    {
        RESPReader reader(*pair.server);
        boost::system::error_code close_ec = (*pair.client).close();
        if (close_ec) {
            out.error = "close failed: " + close_ec.message();
            co_return;
        }
        auto value = co_await reader.read();
        if (value) {
            out.error = "expected EOF got data";
            co_return;
        }
        if (value.error() != boost::asio::error::eof
            && value.error() != boost::asio::error::connection_reset) {
            out.error = "unexpected error: " + value.error().message();
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> connection_raw_read_until_read_exact(LoopbackPair& pair, Captured& out)
    {
        auto written = co_await (*pair.client).write("$5\r\nhello\r\n");
        if (fail(out, written, "write failed: ")) {
            co_return;
        }
        if (written.value() != 11u) {
            out.error = "write byte count mismatch";
            co_return;
        }
        auto line = co_await (*pair.server).read_until("\r\n");
        if (fail(out, line, "read_until failed: ")) {
            co_return;
        }
        if (line.value() != "$5\r\n") {
            out.error = "header line mismatch";
            co_return;
        }
        auto payload = co_await (*pair.server).read_exact(5);
        if (fail(out, payload, "read_exact failed: ")) {
            co_return;
        }
        if (payload.value() != "hello") {
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

    boost::asio::awaitable<void> connection_raw_write_and_read_back(LoopbackPair& pair, Captured& out)
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

    boost::asio::awaitable<void> connection_partial_write_and_read(LoopbackPair& pair, Captured& out)
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

    boost::asio::awaitable<void> read_eof_on_raw_connection(LoopbackPair& pair, Captured& out)
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

    boost::asio::awaitable<void> malformed_prefix_yields_protocol_error(LoopbackPair& pair, Captured& out)
    {
        RESPReader reader(*pair.server);
        auto written = co_await (*pair.client).write("?oops\r\n");
        if (!written) {
            out.error = "write failed: " + written.error().message();
            co_return;
        }
        auto value = co_await reader.read();
        if (value) {
            out.error = "expected protocol_error got data";
            co_return;
        }
        if (value.error() != boost::system::errc::protocol_error) {
            out.error = "unexpected error: " + value.error().message();
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> malformed_length_yields_protocol_error(LoopbackPair& pair, Captured& out)
    {
        RESPReader reader(*pair.server);
        auto written = co_await (*pair.client).write("$abc\r\n");
        if (!written) {
            out.error = "write failed: " + written.error().message();
            co_return;
        }
        auto value = co_await reader.read();
        if (value) {
            out.error = "expected protocol_error got data";
            co_return;
        }
        if (value.error() != boost::system::errc::protocol_error) {
            out.error = "unexpected error: " + value.error().message();
            co_return;
        }
        out.ok = true;
    }

    boost::asio::awaitable<void> overflow_length_yields_protocol_error(LoopbackPair& pair, Captured& out)
    {
        RESPReader reader(*pair.server);
        auto written = co_await (*pair.client).write(":9223372036854775808\r\n");
        if (!written) {
            out.error = "write failed: " + written.error().message();
            co_return;
        }
        auto value = co_await reader.read();
        if (value) {
            out.error = "expected protocol_error got data";
            co_return;
        }
        if (value.error() != boost::system::errc::protocol_error) {
            out.error = "unexpected error: " + value.error().message();
            co_return;
        }
        out.ok = true;
    }

    void run_io(LoopbackPair& pair, boost::asio::awaitable<void> coro)
    {
        boost::asio::co_spawn(pair.ctx, std::move(coro), boost::asio::detached);
        pair.ctx.run();
    }

    TEST(Loopback, WriterReaderBulkString)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, write_then_read(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
        expect_value_equal(out.value, RESPValue::bulk_string("hello"));
    }

    TEST(Loopback, WriterReaderNestedArray)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, write_nested_array(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
        const auto expected = RESPValue::array({
            RESPValue::integer(1),
            RESPValue::bulk_string("a"),
            RESPValue::null(),
            RESPValue::array({ RESPValue::bulk_string("nested") }),
        });
        expect_value_equal(out.value, expected);
    }

    TEST(Loopback, WriterReaderErrorAndInteger)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, write_error_and_integer(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
        const auto expected = RESPValue::array({
            RESPValue::error("ERR", "boom"),
            RESPValue::integer(-123),
        });
        expect_value_equal(out.value, expected);
    }

    TEST(Loopback, ReaderEofWhenPeerCloses)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, read_eof_when_peer_closes(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Loopback, ConnectionReadUntilThenReadExact)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, connection_raw_read_until_read_exact(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Loopback, ConnectionWriteAndReadBack)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, connection_raw_write_and_read_back(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Loopback, ConnectionPartialRead)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, connection_partial_write_and_read(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Loopback, ConnectionEof)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, read_eof_on_raw_connection(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Loopback, MalformedPrefixYieldsProtocolError)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, malformed_prefix_yields_protocol_error(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Loopback, MalformedLengthYieldsProtocolError)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, malformed_length_yields_protocol_error(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

    TEST(Loopback, OverflowLengthYieldsProtocolError)
    {
        LoopbackPair pair;
        Captured out;
        run_io(pair, overflow_length_yields_protocol_error(pair, out));
        ASSERT_TRUE(out.ok) << out.error;
    }

}
}