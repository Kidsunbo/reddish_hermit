#include <common/protocol/protocol.h>

#include <boost/system/errc.hpp>

#include <cstdint>
#include <limits>
#include <string>

namespace reddish::common::protocol {

    std::string encode(const RESPValue& value)
    {
        switch (value.type) {
        case RESPValue::Type::SimpleString:
            return "+" + value.string_value() + "\r\n";
        case RESPValue::Type::Error: {
            const RESPError& error = value.error_value();
            return "-" + error.code + " " + error.message + "\r\n";
        }
        case RESPValue::Type::Integer:
            return ":" + std::to_string(value.integer_value()) + "\r\n";
        case RESPValue::Type::BulkString:
            return "$" + std::to_string(value.string_value().size()) + "\r\n" + value.string_value() + "\r\n";
        case RESPValue::Type::Null:
            return "$-1\r\n";
        case RESPValue::Type::Array: {
            const std::vector<RESPValue>& items = value.array_value();
            std::string result = "*" + std::to_string(items.size()) + "\r\n";
            for (const RESPValue& item : items) {
                result += encode(item);
            }
            return result;
        }
        default:
            return "";
        }
    }

    namespace {

        utils::SyncResult<std::string_view> strip_line(std::string_view line)
        {
            if (line.size() < 3 || line[line.size() - 2] != '\r' || line[line.size() - 1] != '\n') {
                return boost::system::errc::protocol_error;
            }
            return line.substr(1, line.size() - 3);
        }

        utils::SyncResult<std::int64_t> parse_int(std::string_view text)
        {
            if (text.empty()) {
                return boost::system::errc::protocol_error;
            }
            bool negative = false;
            std::size_t index = 0;
            if (text[0] == '-') {
                negative = true;
                ++index;
            } else if (text[0] == '+') {
                ++index;
            }
            if (index >= text.size()) {
                return boost::system::errc::protocol_error;
            }
            const std::uint64_t limit = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())
                + (negative ? 1u : 0u);
            std::uint64_t magnitude = 0;
            for (; index < text.size(); ++index) {
                const char c = text[index];
                if (c < '0' || c > '9') {
                    return boost::system::errc::protocol_error;
                }
                const std::uint64_t digit = static_cast<std::uint64_t>(c - '0');
                if (magnitude > (limit - digit) / 10) {
                    return boost::system::errc::protocol_error;
                }
                magnitude = magnitude * 10 + digit;
            }
            if (negative) {
                if (magnitude == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1) {
                    return std::numeric_limits<std::int64_t>::min();
                }
                return -static_cast<std::int64_t>(magnitude);
            }
            return static_cast<std::int64_t>(magnitude);
        }

    }

    utils::SyncResult<RESPValue> parse_simple_string(std::string_view line)
    {
        auto content = strip_line(line);
        if (!content) {
            return content.error();
        }
        return RESPValue::simple_string(std::string(content.value()));
    }

    utils::SyncResult<RESPValue> parse_error(std::string_view line)
    {
        auto content = strip_line(line);
        if (!content) {
            return content.error();
        }
        const std::string_view text = content.value();
        const std::size_t space = text.find(' ');
        if (space == std::string_view::npos) {
            return RESPValue::error(std::string(text), "");
        }
        return RESPValue::error(std::string(text.substr(0, space)), std::string(text.substr(space + 1)));
    }

    utils::SyncResult<RESPValue> parse_integer(std::string_view line)
    {
        auto content = strip_line(line);
        if (!content) {
            return content.error();
        }
        auto value = parse_int(content.value());
        if (!value) {
            return value.error();
        }
        return RESPValue::integer(value.value());
    }

    utils::SyncResult<std::int64_t> parse_length(std::string_view line)
    {
        auto content = strip_line(line);
        if (!content) {
            return content.error();
        }
        auto value = parse_int(content.value());
        if (!value) {
            return value.error();
        }
        if (value.value() < -1) {
            return boost::system::errc::protocol_error;
        }
        return value.value();
    }

    std::optional<RESP2Type> resp2_type_from_prefix(char prefix) noexcept
    {
        switch (prefix) {
        case '-':
            return RESP2Type::Error;
        case '+':
            return RESP2Type::SimpleString;
        case ':':
            return RESP2Type::Integer;
        case '$':
            return RESP2Type::BulkString;
        case '*':
            return RESP2Type::Array;
        default:
            return std::nullopt;
        }
    }

}
