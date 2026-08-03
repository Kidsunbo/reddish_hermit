#include <common/protocol/resp_value.h>

#include <utility>

namespace reddish::common::protocol {

    RESPValue RESPValue::simple_string(std::string value)
    {
        return { Type::SimpleString, std::move(value) };
    }

    RESPValue RESPValue::error(std::string code, std::string message)
    {
        return { Type::Error, RESPError { std::move(code), std::move(message) } };
    }

    RESPValue RESPValue::integer(std::int64_t value)
    {
        return { Type::Integer, value };
    }

    RESPValue RESPValue::bulk_string(std::string value)
    {
        return { Type::BulkString, std::move(value) };
    }

    RESPValue RESPValue::null()
    {
        return { Type::Null, std::monostate {} };
    }

    RESPValue RESPValue::array(std::vector<RESPValue> items)
    {
        return { Type::Array, std::move(items) };
    }

    bool RESPValue::is_null() const
    {
        return type == Type::Null;
    }

    const std::string& RESPValue::string_value() const
    {
        return std::get<std::string>(data);
    }

    std::int64_t RESPValue::integer_value() const
    {
        return std::get<std::int64_t>(data);
    }

    const RESPError& RESPValue::error_value() const
    {
        return std::get<RESPError>(data);
    }

    const std::vector<RESPValue>& RESPValue::array_value() const
    {
        return std::get<std::vector<RESPValue>>(data);
    }

}
