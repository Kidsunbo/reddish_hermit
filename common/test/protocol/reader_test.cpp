#include <boost/system/errc.hpp>
#include <common/protocol/protocol.h>
#include <common/protocol/resp_value.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <string>

namespace reddish::common::protocol {
namespace {

    using utils::SyncResult;

    TEST(Reader, ParseSimpleString)
    {
        auto value = parse_simple_string("+OK\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().type, RESPValue::Type::SimpleString);
        EXPECT_EQ(value.value().string_value(), "OK");
    }

    TEST(Reader, ParseSimpleStringEmpty)
    {
        auto value = parse_simple_string("+\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().type, RESPValue::Type::SimpleString);
        EXPECT_EQ(value.value().string_value(), "");
    }

    TEST(Reader, ParseSimpleStringWithoutTerminator)
    {
        auto value = parse_simple_string("+OK");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseErrorCodeAndMessage)
    {
        auto value = parse_error("-ERR msg\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().type, RESPValue::Type::Error);
        EXPECT_EQ(value.value().error_value().code, "ERR");
        EXPECT_EQ(value.value().error_value().message, "msg");
    }

    TEST(Reader, ParseErrorMultiwordMessage)
    {
        auto value = parse_error("-WRONGTYPE Operation against a key holding the wrong kind of value\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().error_value().code, "WRONGTYPE");
        EXPECT_EQ(value.value().error_value().message, "Operation against a key holding the wrong kind of value");
    }

    TEST(Reader, ParseErrorNoMessage)
    {
        auto value = parse_error("-ERR\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().error_value().code, "ERR");
        EXPECT_EQ(value.value().error_value().message, "");
    }

    TEST(Reader, ParseIntegerPositive)
    {
        auto value = parse_integer(":42\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().type, RESPValue::Type::Integer);
        EXPECT_EQ(value.value().integer_value(), 42);
    }

    TEST(Reader, ParseIntegerNegative)
    {
        auto value = parse_integer(":-123\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().type, RESPValue::Type::Integer);
        EXPECT_EQ(value.value().integer_value(), -123);
    }

    TEST(Reader, ParseIntegerZero)
    {
        auto value = parse_integer(":0\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().integer_value(), 0);
    }

    TEST(Reader, ParseIntegerMin)
    {
        auto value = parse_integer(":-9223372036854775808\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().integer_value(), INT64_MIN);
    }

    TEST(Reader, ParseIntegerMax)
    {
        auto value = parse_integer(":9223372036854775807\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().integer_value(), INT64_MAX);
    }

    TEST(Reader, ParseIntegerOverflow)
    {
        auto value = parse_integer(":9223372036854775808\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseIntegerUnderflow)
    {
        auto value = parse_integer(":-9223372036854775809\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseIntegerGarbage)
    {
        auto value = parse_integer(":abc\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseIntegerTrailingJunk)
    {
        auto value = parse_integer(":12a3\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseIntegerEmpty)
    {
        auto value = parse_integer(":\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseIntegerExplicitPlus)
    {
        auto value = parse_integer(":+5\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value().integer_value(), 5);
    }

    TEST(Reader, ParseIntegerLoneSign)
    {
        auto value = parse_integer(":-\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseLengthPositive)
    {
        auto value = parse_length("$5\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value(), 5);
    }

    TEST(Reader, ParseLengthNullBulk)
    {
        auto value = parse_length("$-1\r\n");
        ASSERT_TRUE(value);
        EXPECT_EQ(value.value(), -1);
    }

    TEST(Reader, ParseLengthNegativeOther)
    {
        auto value = parse_length("$-2\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseLengthNonNumeric)
    {
        auto value = parse_length("$abc\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, ParseLengthOverflow)
    {
        auto value = parse_length("$9223372036854775808\r\n");
        ASSERT_FALSE(value);
        EXPECT_EQ(value.error(), boost::system::errc::protocol_error);
    }

    TEST(Reader, PrefixDispatchValid)
    {
        EXPECT_EQ(resp2_type_from_prefix('+'), RESP2Type::SimpleString);
        EXPECT_EQ(resp2_type_from_prefix('-'), RESP2Type::Error);
        EXPECT_EQ(resp2_type_from_prefix(':'), RESP2Type::Integer);
        EXPECT_EQ(resp2_type_from_prefix('$'), RESP2Type::BulkString);
        EXPECT_EQ(resp2_type_from_prefix('*'), RESP2Type::Array);
    }

    TEST(Reader, PrefixDispatchInvalid)
    {
        EXPECT_FALSE(resp2_type_from_prefix('\0').has_value());
        EXPECT_FALSE(resp2_type_from_prefix('x').has_value());
        EXPECT_FALSE(resp2_type_from_prefix('\r').has_value());
    }

}
}
