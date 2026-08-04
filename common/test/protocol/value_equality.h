#pragma once

#include <common/protocol/resp_value.h>
#include <gtest/gtest.h>

#include <cstddef>

namespace reddish::common::protocol::test {

    inline void expect_value_equal(const RESPValue& actual, const RESPValue& expected)
    {
        ASSERT_EQ(actual.type, expected.type);
        switch (actual.type) {
        case RESPValue::Type::SimpleString:
            EXPECT_EQ(actual.string_value(), expected.string_value());
            break;
        case RESPValue::Type::Error:
            EXPECT_EQ(actual.error_value().code, expected.error_value().code);
            EXPECT_EQ(actual.error_value().message, expected.error_value().message);
            break;
        case RESPValue::Type::Integer:
            EXPECT_EQ(actual.integer_value(), expected.integer_value());
            break;
        case RESPValue::Type::BulkString:
            EXPECT_EQ(actual.string_value(), expected.string_value());
            break;
        case RESPValue::Type::Null:
            EXPECT_TRUE(expected.is_null());
            break;
        case RESPValue::Type::Array:
            ASSERT_EQ(actual.array_value().size(), expected.array_value().size());
            for (std::size_t i = 0; i < actual.array_value().size(); ++i) {
                expect_value_equal(actual.array_value()[i], expected.array_value()[i]);
            }
            break;
        }
    }

}