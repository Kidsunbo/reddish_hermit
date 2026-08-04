#include <boost/system/errc.hpp>
#include <common/protocol/protocol.h>
#include <common/protocol/resp_value.h>
#include <protocol/value_equality.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace reddish::common::protocol {
namespace {

    using test::expect_value_equal;

    // A small synchronous decoder used only by this test. It consumes bytes
    // through the pure parse helpers, mirroring the awaitable RESPReader's
    // line/payload split so the full type universe round-trips without sockets.
    class TestDecoder {
    public:
        explicit TestDecoder(std::string_view bytes)
            : bytes_(bytes)
        {
        }

        utils::SyncResult<RESPValue> decode()
        {
            auto line = read_line();
            if (!line) {
                return line.error();
            }

            const std::string_view text = line.value();
            if (text.empty()) {
                return boost::system::errc::protocol_error;
            }

            switch (text[0]) {
            case '+': {
                auto parsed = parse_simple_string(text);
                if (!parsed) {
                    return parsed.error();
                }
                return parsed.value();
            }
            case '-': {
                auto parsed = parse_error(text);
                if (!parsed) {
                    return parsed.error();
                }
                return parsed.value();
            }
            case ':': {
                auto parsed = parse_integer(text);
                if (!parsed) {
                    return parsed.error();
                }
                return parsed.value();
            }
            case '$': {
                auto parsed = parse_length(text);
                if (!parsed) {
                    return parsed.error();
                }
                const std::int64_t length = parsed.value();
                if (length == -1) {
                    return RESPValue::null();
                }
                auto payload = read_exact(static_cast<std::uint64_t>(length) + 2);
                if (!payload) {
                    return payload.error();
                }
                const std::string_view bytes = payload.value();
                if (bytes.size() < static_cast<std::size_t>(length) + 2 || bytes[bytes.size() - 2] != '\r'
                    || bytes[bytes.size() - 1] != '\n') {
                    return boost::system::errc::protocol_error;
                }
                return RESPValue::bulk_string(std::string(bytes.substr(0, static_cast<std::size_t>(length))));
            }
            case '*': {
                auto parsed = parse_length(text);
                if (!parsed) {
                    return parsed.error();
                }
                const std::int64_t length = parsed.value();
                if (length == -1) {
                    return RESPValue::null();
                }
                std::vector<RESPValue> items;
                items.reserve(static_cast<std::size_t>(length));
                for (std::int64_t i = 0; i < length; ++i) {
                    auto item = decode();
                    if (!item) {
                        return item.error();
                    }
                    items.push_back(std::move(item).value());
                }
                return RESPValue::array(std::move(items));
            }
            default:
                return boost::system::errc::protocol_error;
            }
        }

    private:
        utils::SyncResult<std::string_view> read_line()
        {
            const std::size_t end = bytes_.find("\r\n");
            if (end == std::string_view::npos) {
                return boost::system::errc::protocol_error;
            }
            std::string_view result = bytes_.substr(0, end + 2);
            bytes_.remove_prefix(result.size());
            return result;
        }

        utils::SyncResult<std::string_view> read_exact(std::size_t size)
        {
            if (bytes_.size() < size) {
                return boost::system::errc::protocol_error;
            }
            std::string_view result = bytes_.substr(0, size);
            bytes_.remove_prefix(size);
            return result;
        }

        std::string_view bytes_;
    };

    utils::SyncResult<RESPValue> decode(std::string_view bytes)
    {
        TestDecoder decoder(bytes);
        return decoder.decode();
    }

    TEST(Roundtrip, SimpleString)
    {
        const auto original = RESPValue::simple_string("OK");
        const auto bytes = encode(original);
        auto decoded = decode(bytes);
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, SimpleStringEmpty)
    {
        const auto original = RESPValue::simple_string("");
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, Error)
    {
        const auto original = RESPValue::error("ERR", "unknown command 'FOO'");
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, ErrorNoMessage)
    {
        const auto original = RESPValue::error("WRONGTYPE", "");
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, Integer)
    {
        const auto original = RESPValue::integer(-123);
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, IntegerZero)
    {
        const auto original = RESPValue::integer(0);
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, BulkString)
    {
        const auto original = RESPValue::bulk_string("hello");
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, BulkStringEmpty)
    {
        const auto original = RESPValue::bulk_string("");
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, BulkStringBinary)
    {
        const auto original = RESPValue::bulk_string(std::string("\x00\x01\xfe\xff", 4));
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, Null)
    {
        const auto original = RESPValue::null();
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, EmptyArray)
    {
        const auto original = RESPValue::array({});
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, ArrayOfBulkStrings)
    {
        const auto original = RESPValue::array({
            RESPValue::bulk_string("GET"),
            RESPValue::bulk_string("key"),
        });
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, NestedArrayPreservesTypes)
    {
        const auto original = RESPValue::array({
            RESPValue::integer(1),
            RESPValue::array({
                RESPValue::bulk_string("nested"),
                RESPValue::error("ERR", "inner"),
            }),
            RESPValue::null(),
            RESPValue::simple_string("s"),
        });
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

    TEST(Roundtrip, ArrayWithNullElements)
    {
        const auto original = RESPValue::array({
            RESPValue::null(),
            RESPValue::bulk_string("x"),
            RESPValue::null(),
        });
        auto decoded = decode(encode(original));
        ASSERT_TRUE(decoded);
        expect_value_equal(decoded.value(), original);
    }

}
}
