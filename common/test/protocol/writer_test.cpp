#include <common/command/command.h>
#include <common/protocol/protocol.h>
#include <common/protocol/resp_value.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

namespace reddish::common::protocol {
namespace {

    TEST(Writer, EncodeSimpleString)
    {
        EXPECT_EQ(encode(RESPValue::simple_string("OK")), "+OK\r\n");
    }

    TEST(Writer, EncodeSimpleStringEmpty)
    {
        EXPECT_EQ(encode(RESPValue::simple_string("")), "+\r\n");
    }

    TEST(Writer, EncodeError)
    {
        EXPECT_EQ(encode(RESPValue::error("ERR", "unknown command")), "-ERR unknown command\r\n");
    }

    TEST(Writer, EncodeErrorNoMessage)
    {
        EXPECT_EQ(encode(RESPValue::error("ERR", "")), "-ERR \r\n");
    }

    TEST(Writer, EncodeInteger)
    {
        EXPECT_EQ(encode(RESPValue::integer(42)), ":42\r\n");
    }

    TEST(Writer, EncodeIntegerNegative)
    {
        EXPECT_EQ(encode(RESPValue::integer(-123)), ":-123\r\n");
    }

    TEST(Writer, EncodeBulkString)
    {
        EXPECT_EQ(encode(RESPValue::bulk_string("hello")), "$5\r\nhello\r\n");
    }

    TEST(Writer, EncodeBulkStringEmpty)
    {
        EXPECT_EQ(encode(RESPValue::bulk_string("")), "$0\r\n\r\n");
    }

    TEST(Writer, EncodeBulkStringBinary)
    {
        const std::string payload = std::string("\x00\x01\x02\xff", 4);
        const std::string expected = std::string("$4\r\n\x00\x01\x02\xff\r\n", 10);
        EXPECT_EQ(encode(RESPValue::bulk_string(payload)), expected);
    }

    TEST(Writer, EncodeBulkStringSizeMatchesBytesNotChars)
    {
        const std::string payload = std::string("\xff\xfe\xfd", 3);
        const std::string expected = std::string("$3\r\n\xff\xfe\xfd\r\n", 9);
        EXPECT_EQ(encode(RESPValue::bulk_string(payload)), expected);
    }

    TEST(Writer, EncodeNull)
    {
        EXPECT_EQ(encode(RESPValue::null()), "$-1\r\n");
    }

    TEST(Writer, EncodeEmptyArray)
    {
        EXPECT_EQ(encode(RESPValue::array({})), "*0\r\n");
    }

    TEST(Writer, EncodeArrayOfBulkStrings)
    {
        const auto value = RESPValue::array({
            RESPValue::bulk_string("GET"),
            RESPValue::bulk_string("key"),
        });
        EXPECT_EQ(encode(value), "*2\r\n$3\r\nGET\r\n$3\r\nkey\r\n");
    }

    TEST(Writer, EncodeNestedArray)
    {
        const auto inner = RESPValue::array({
            RESPValue::integer(1),
            RESPValue::bulk_string("a"),
        });
        const auto outer = RESPValue::array({
            RESPValue::simple_string("+"),
            inner,
        });
        EXPECT_EQ(encode(outer), "*2\r\n++\r\n*2\r\n:1\r\n$1\r\na\r\n");
    }

    TEST(Writer, EncodeArrayWithNull)
    {
        const auto value = RESPValue::array({
            RESPValue::null(),
            RESPValue::bulk_string("x"),
        });
        EXPECT_EQ(encode(value), "*2\r\n$-1\r\n$1\r\nx\r\n");
    }

    TEST(Writer, GetRequestFraming)
    {
        EXPECT_EQ(commands::Command<commands::CommandEnum::Get>::to_string("key"),
            "*2\r\n$3\r\nGET\r\n$3\r\nkey\r\n");
    }

    TEST(Writer, SetRequestFramingWithOptions)
    {
        EXPECT_EQ(commands::Command<commands::CommandEnum::Set>::to_string("key", "value", 10, 20),
            "*5\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n$2\r\n10\r\n$2\r\n20\r\n");
    }

    TEST(Writer, GetBitRequestFraming)
    {
        EXPECT_EQ(commands::Command<commands::CommandEnum::GetBit>::to_string("key"),
            "*2\r\n$6\r\nGETBIT\r\n$3\r\nkey\r\n");
    }

}
}
