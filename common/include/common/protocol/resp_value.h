#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace reddish::common::protocol {

    enum class RESP2Type : char {
        Error = '-',
        SimpleString = '+',
        Integer = ':',
        BulkString = '$',
        Array = '*',
    };

    struct RESPError {
        std::string code;
        std::string message;
    };

    struct RESPValue {
        enum class Type {
            SimpleString,
            Error,
            Integer,
            BulkString,
            Null,
            Array,
        };

        using Payload =
            std::variant<std::monostate, std::string, std::int64_t, RESPError, std::vector<RESPValue>>;

        Type type { Type::Null };
        Payload data;

        static RESPValue simple_string(std::string value);
        static RESPValue error(std::string code, std::string message);
        static RESPValue integer(std::int64_t value);
        static RESPValue bulk_string(std::string value);
        static RESPValue null();
        static RESPValue array(std::vector<RESPValue> items);

        [[nodiscard]] bool is_null() const;
        [[nodiscard]] const std::string& string_value() const;
        [[nodiscard]] std::int64_t integer_value() const;
        [[nodiscard]] const RESPError& error_value() const;
        [[nodiscard]] const std::vector<RESPValue>& array_value() const;
    };

}
