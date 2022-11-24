#include "reddish/resp/protocol.h"
#include "reddish/utils/str.h"

#include <sstream>
#include <charconv>

namespace reddish::resp
{

    template <typename T>
    concept specific_result_concept = requires(T t, network::Connection conn)
    {
        typename T::value_type;
        {T::create_from_connection(conn)} -> std::same_as<boost::outcome_v2::result<T>>;
        {t.result()} -> std::same_as<boost::outcome_v2::result<typename T::value_type> &>;
        {t.result(std::declval<typename T::value_type>())} -> std::same_as<typename T::value_type>;
    };

    static_assert(specific_result_concept<IntResult>, "IntResult does not satisfy the requirement");
    static_assert(specific_result_concept<StringResult>, "StringResult does not satisfy the requirement");
    static_assert(specific_result_concept<BoolResult>, "BoolResult does not satisfy the requirement");
    static_assert(specific_result_concept<VectorResult>, "VectorResult does not satisfy the requirement");
    static_assert(specific_result_concept<DoubleResult>, "DoubleResult does not satisfy the requirement");
    static_assert(specific_result_concept<DoubleVectorResult>, "DoubleResult does not satisfy the requirement");
    static_assert(specific_result_concept<IntVectorResult>, "IntVectorResult does not satisfy the requirement");
    static_assert(specific_result_concept<StringVectorResult>, "StringVectorResult does not satisfy the requirement");
    static_assert(specific_result_concept<BoolVectorResult>, "BoolVectorResult does not satisfy the requirement");
    static_assert(specific_result_concept<MapResult>, "MapResult does not satisfy the requirement");
    static_assert(specific_result_concept<MapStringStringResult>, "MapStringStringResult does not satisfy the requirement");
    static_assert(specific_result_concept<MapStringIntResult>, "MapStringIntResult does not satisfy the requirement");

    std::string Request::item_wrap(std::string_view value)
    {
        std::string wrapped_str;
        // Assume that the wrapped str is '$<five number>\r\n{value}\r\n'.
        const std::size_t length = 10 + value.length();
        wrapped_str.reserve(length);
        wrapped_str.append("$");
        wrapped_str.append(std::to_string(value.length()));
        wrapped_str.append("\r\n");
        wrapped_str.append(value);
        wrapped_str.append("\r\n");
        return wrapped_str;
    }

    std::string Request::item_wrap(float value)
    {
        std::stringstream ss;
        ss << value;
        return item_wrap(ss.str());
    }

    std::string Request::item_wrap(double value)
    {
        std::stringstream ss;
        ss << value;
        return item_wrap(ss.str());
    }

    std::string Request::item_wrap(std::int64_t value)
    {
        return item_wrap(std::to_string(value));
    }

    std::string Request::item_wrap(std::uint64_t value)
    {
        return item_wrap(std::to_string(value));
    }

    std::string Request::item_wrap(std::int32_t value)
    {
        return item_wrap(std::to_string(value));
    }

    std::string Request::item_wrap(std::uint32_t value)
    {
        return item_wrap(std::to_string(value));
    }

    std::string Request::item_wrap(bool value)
    {
        return value ? item_wrap("true") : item_wrap("false");
    }

    std::string Request::item_wrap(const char *value)
    {
        return item_wrap(std::string_view{value});
    }

    std::string Request::item_wrap(long double value)
    {
        std::stringstream ss;
        ss << value;
        return item_wrap(ss.str());
    }

    std::string_view Request::to_string_view()
    {
        return request_str;
    }

    std::string Request::to_string()
    {
        return request_str;
    }

    boost::outcome_v2::result<Result> Result::from_string(const std::string &s)
    {
        if (s.length() < 3 || (s[s.length() - 2] != '\r' || s[s.length() - 1] != '\n'))
        {
            return boost::system::errc::invalid_argument;
        }
        Result result;
        result.val = s;
        return result;
    }

    boost::outcome_v2::result<Result> Result::from_string(std::string &&s)
    {
        if (s.length() < 3 || (s[s.length() - 2] != '\r' || s[s.length() - 1] != '\n'))
        {
            return boost::system::errc::invalid_argument;
        }
        Result result;
        result.val = std::move(s);
        return result;
    }

    Result::ResultType Result::type() const noexcept
    {
        if(val.length() == 0){
            return ResultType::Unknown;
        }
        switch (val[0])
        {
        case '+':
            return ResultType::SimpleString;
        case '-':
            return ResultType::Error;
        case ':':
            return ResultType::Integer;
        case '$':
            return ResultType::BulkString;
        case '*':
            return ResultType::Array;
        }
        return ResultType::Unknown;
    }

    bool Result::is_null() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::BulkString)
        {
            auto it = val.cbegin() + 1;
            if (utils::advance_if_same(it, val.cend(), "-1"))
            {
                return true;
            }
        }
        return false;
    }

    boost::outcome_v2::result<std::string> Result::as_string() const noexcept
    {
        switch (type())
        {
        case ResultType::SimpleString:
        case ResultType::Integer:
        case ResultType::Error:
            return val.substr(1, val.length() - 3);
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length)
            {
                return utils::retrieve_string_without_advance(it, val.cend(), length.value());
            }
            else
            {
                return length.error();
            }
        }
        default:
            return boost::system::errc::wrong_protocol_type;
        }
    }

    boost::outcome_v2::result<std::int32_t> Result::as_int32() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::Error)
        {
            return boost::system::errc::invalid_argument;
        }
        switch (type())
        {
        case ResultType::SimpleString:
        case ResultType::Integer:
        {
            std::int32_t result;
            [[maybe_unused]] auto [ignore, ec] = std::from_chars(val.data() + 1, val.data() + val.length() - 2, result);
            if (ec != std::errc{})
            {
                return boost::system::errc::invalid_argument;
            }
            return result;
        }
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length && length.value() > 0 && std::distance(it, val.cend()) >= length.value() + 2)
            {
                std::int32_t result;
                [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base(), it.base() + length.value(), result);
                if (ec != std::errc{})
                {
                    return boost::system::errc::invalid_argument;
                }
                return result;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

    boost::outcome_v2::result<std::int64_t> Result::as_int64() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::Error)
        {
            return boost::system::errc::invalid_argument;
        }
        switch (type())
        {
        case ResultType::SimpleString:
        case ResultType::Integer:
        {
            std::int64_t result;
            [[maybe_unused]] auto [ignore, ec] = std::from_chars(val.data() + 1, val.data() + val.length() - 2, result);
            if (ec != std::errc{})
            {
                return boost::system::errc::invalid_argument;
            }
            return result;
        }
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length && length.value() > 0 && std::distance(it, val.cend()) >= length.value() + 2)
            {
                std::int64_t result;
                [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base(), it.base() + length.value(), result);
                if (ec != std::errc{})
                {
                    return boost::system::errc::invalid_argument;
                }
                return result;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

    boost::outcome_v2::result<std::uint32_t> Result::as_uint32() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::Error)
        {
            return boost::system::errc::invalid_argument;
        }
        switch (type())
        {
        case ResultType::SimpleString:
        case ResultType::Integer:
        {
            std::uint32_t result;
            [[maybe_unused]] auto [ignore, ec] = std::from_chars(val.data() + 1, val.data() + val.length() - 2, result);
            if (ec != std::errc{})
            {
                return boost::system::errc::invalid_argument;
            }
            return result;
        }
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length && length.value() > 0 && std::distance(it, val.cend()) >= length.value() + 2)
            {
                std::uint32_t result;
                [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base(), it.base() + length.value(), result);
                if (ec != std::errc{})
                {
                    return boost::system::errc::invalid_argument;
                }
                return result;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

    boost::outcome_v2::result<std::uint64_t> Result::as_uint64() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::Error)
        {
            return boost::system::errc::invalid_argument;
        }
        switch (type())
        {
        case ResultType::SimpleString:
        case ResultType::Integer:
        {
            std::uint64_t result;
            [[maybe_unused]] auto [ignore, ec] = std::from_chars(val.data() + 1, val.data() + val.length() - 2, result);
            if (ec != std::errc{})
            {
                return boost::system::errc::invalid_argument;
            }
            return result;
        }
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length && length.value() > 0 && std::distance(it, val.cend()) >= length.value() + 2)
            {
                std::uint64_t result;
                [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base(), it.base() + length.value(), result);
                if (ec != std::errc{})
                {
                    return boost::system::errc::invalid_argument;
                }
                return result;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

    boost::outcome_v2::result<bool> Result::as_boolean() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::Error)
        {
            return boost::system::errc::invalid_argument;
        }
        switch (type())
        {
        case ResultType::Integer:
        case ResultType::SimpleString:
        {
            if (utils::case_insensitive_equal(std::string_view{val}.substr(1), "true\r\n") || utils::case_insensitive_equal(std::string_view{val}.substr(1), "1\r\n") || utils::case_insensitive_equal(std::string_view{val}.substr(1), "t\r\n"))
            {
                return true;
            }
            return false;
        }
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length && length.value() > 0 && std::distance(it, val.cend()) >= length.value() + 2)
            {
                if (utils::case_insensitive_equal(std::string_view{it, it + length.value()}, "true") || utils::case_insensitive_equal(std::string_view{it, it + length.value()}, "1") || utils::case_insensitive_equal(std::string_view{it, it + length.value()}, "t"))
                {
                    return true;
                }
                return false;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

    boost::outcome_v2::result<float> Result::as_float() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::Error)
        {
            return boost::system::errc::invalid_argument;
        }
        switch (type())
        {
        case ResultType::SimpleString:
        case ResultType::Integer:
        {
            float result;
            [[maybe_unused]] auto [ignore, ec] = std::from_chars(val.data() + 1, val.data() + val.length() - 2, result);
            if (ec != std::errc{})
            {
                return boost::system::errc::invalid_argument;
            }
            return result;
        }
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length && length.value() > 0 && std::distance(it, val.cend()) >= length.value() + 2)
            {
                float result;
                [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base(), it.base() + length.value(), result);
                if (ec != std::errc{})
                {
                    return boost::system::errc::invalid_argument;
                }
                return result;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

    boost::outcome_v2::result<double> Result::as_double() const noexcept
    {
        if (type() == ResultType::Array || type() == ResultType::Error)
        {
            return boost::system::errc::invalid_argument;
        }
        switch (type())
        {
        case ResultType::SimpleString:
        case ResultType::Integer:
        {
            double result;
            [[maybe_unused]] auto [ignore, ec] = std::from_chars(val.data() + 1, val.data() + val.length() - 2, result);
            if (ec != std::errc{})
            {
                return boost::system::errc::invalid_argument;
            }
            return result;
        }
        case ResultType::BulkString:
        {
            auto it = val.cbegin() + 1;
            auto length = utils::retrieve_length(it, val.cend());
            if (length && length.value() > 0 && std::distance(it, val.cend()) >= length.value() + 2)
            {
                double result;
                [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base(), it.base() + length.value(), result);
                if (ec != std::errc{})
                {
                    return boost::system::errc::invalid_argument;
                }
                return result;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

    boost::outcome_v2::result<std::vector<Result>> Result::as_vector() const noexcept
    {
        if (type() != ResultType::Array)
        {
            return boost::system::errc::invalid_argument;
        }
        auto it = val.cbegin() + 1;
        auto length = utils::retrieve_length(it, val.cend());
        if (!length)
        {
            return boost::system::errc::invalid_argument;
        }
        std::vector<Result> vec;
        vec.reserve(length.value());
        for (std::int64_t i = 0; i < length.value(); i++)
        {
            auto length = utils::next_item_length(it, val.cend());
            if (!length)
            {
                return length.error();
            }
            auto result = Result::from_string(std::string{it, it + length.value()});
            if (result)
            {
                vec.push_back(result.value());
                it += length.value();
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
        }
        return vec;
    }

    boost::outcome_v2::result<std::vector<std::string>> Result::as_string_vector() const noexcept
    {
        if (type() != ResultType::Array)
        {
            return boost::system::errc::invalid_argument;
        }
        auto it = val.cbegin() + 1;
        auto length = utils::retrieve_length(it, val.cend());
        if (!length)
        {
            return boost::system::errc::invalid_argument;
        }
        std::vector<std::string> vec;
        vec.reserve(length.value());
        for (std::int64_t i = 0; i < length.value(); i++)
        {
            auto length = utils::next_item_length(it, val.cend());
            if (length && length.value() > 2)
            {
                switch (*it)
                {
                case '+':
                case '-':
                case ':':
                    vec.push_back(std::string{it + 1, it + length.value() - 2});
                    break;
                case '$':
                {
                    auto parse_it = it + 1;
                    auto length = utils::retrieve_length(parse_it, val.cend());
                    if (!length)
                    {
                        return length.error();
                    }
                    auto s = utils::retrieve_string_without_advance(parse_it, val.cend(), length.value());
                    if (!s)
                    {
                        return s.error();
                    }
                    vec.push_back(s.value());
                    break;
                }
                case '*':
                    return boost::system::errc::invalid_argument;
                }
                it += length.value();
            }
            else
            {
                return length.error();
            }
        }
        return vec;
    }

    boost::outcome_v2::result<std::vector<std::int64_t>> Result::as_int64_vector() const noexcept
    {
        if (type() != ResultType::Array)
        {
            return boost::system::errc::invalid_argument;
        }
        auto it = val.cbegin() + 1;
        auto length = utils::retrieve_length(it, val.cend());
        if (!length)
        {
            return boost::system::errc::invalid_argument;
        }
        std::vector<std::int64_t> vec;
        vec.reserve(length.value());
        for (std::int64_t i = 0; i < length.value(); i++)
        {
            auto length = utils::next_item_length(it, val.cend());
            if (length && length.value() > 2)
            {
                switch (*it)
                {
                case '+':
                case ':':
                {
                    std::int64_t result;
                    [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base() + 1, it.base() + length.value() - 2, result);
                    if (ec != std::errc{})
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    vec.push_back(result);
                    break;
                }
                case '$':
                {
                    auto parse_it = it + 1;
                    auto length = utils::retrieve_length(parse_it, val.cend());
                    if (length && length.value() > 0 && std::distance(parse_it, val.cend()) >= length.value() + 2)
                    {
                        std::int64_t result;
                        [[maybe_unused]] auto [ignore, ec] = std::from_chars(parse_it.base(), parse_it.base() + length.value(), result);
                        if (ec != std::errc{})
                        {
                            return boost::system::errc::invalid_argument;
                        }
                        vec.push_back(result);
                    }
                    else
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    break;
                }
                case '-':
                case '*':
                    return boost::system::errc::invalid_argument;
                }
                it += length.value();
            }
            else
            {
                return length.error();
            }
        }
        return vec;
    }

    boost::outcome_v2::result<std::vector<std::uint64_t>> Result::as_uint64_vector() const noexcept
    {
        if (type() != ResultType::Array)
        {
            return boost::system::errc::invalid_argument;
        }
        auto it = val.cbegin() + 1;
        auto length = utils::retrieve_length(it, val.cend());
        if (!length)
        {
            return boost::system::errc::invalid_argument;
        }
        std::vector<std::uint64_t> vec;
        vec.reserve(length.value());
        for (std::int64_t i = 0; i < length.value(); i++)
        {
            auto length = utils::next_item_length(it, val.cend());
            if (length && length.value() > 2)
            {
                switch (*it)
                {
                case '+':
                case ':':
                {
                    std::uint64_t result;
                    [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base() + 1, it.base() + length.value() - 2, result);
                    if (ec != std::errc{})
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    vec.push_back(result);
                    break;
                }
                case '$':
                {
                    auto parse_it = it + 1;
                    auto length = utils::retrieve_length(parse_it, val.cend());
                    if (length && length.value() > 0 && std::distance(parse_it, val.cend()) >= length.value() + 2)
                    {
                        std::uint64_t result;
                        [[maybe_unused]] auto [ignore, ec] = std::from_chars(parse_it.base(), parse_it.base() + length.value(), result);
                        if (ec != std::errc{})
                        {
                            return boost::system::errc::invalid_argument;
                        }
                        vec.push_back(result);
                    }
                    else
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    break;
                }
                case '-':
                case '*':
                    return boost::system::errc::invalid_argument;
                }
                it += length.value();
            }
            else
            {
                return length.error();
            }
        }
        return vec;
    }

    boost::outcome_v2::result<std::vector<float>> Result::as_float_vector() const noexcept
    {
        if (type() != ResultType::Array)
        {
            return boost::system::errc::invalid_argument;
        }
        auto it = val.cbegin() + 1;
        auto length = utils::retrieve_length(it, val.cend());
        if (!length)
        {
            return boost::system::errc::invalid_argument;
        }
        std::vector<float> vec;
        vec.reserve(length.value());
        for (std::int64_t i = 0; i < length.value(); i++)
        {
            auto length = utils::next_item_length(it, val.cend());
            if (length && length.value() > 2)
            {
                switch (*it)
                {
                case '+':
                case ':':
                {
                    float result;
                    [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base() + 1, it.base() + length.value() - 2, result);
                    if (ec != std::errc{})
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    vec.push_back(result);
                    break;
                }
                case '$':
                {
                    auto parse_it = it + 1;
                    auto length = utils::retrieve_length(parse_it, val.cend());
                    if (length && length.value() > 0 && std::distance(parse_it, val.cend()) >= length.value() + 2)
                    {
                        float result;
                        [[maybe_unused]] auto [ignore, ec] = std::from_chars(parse_it.base(), parse_it.base() + length.value(), result);
                        if (ec != std::errc{})
                        {
                            return boost::system::errc::invalid_argument;
                        }
                        vec.push_back(result);
                    }
                    else
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    break;
                }
                case '-':
                case '*':
                    return boost::system::errc::invalid_argument;
                }
                it += length.value();
            }
            else
            {
                return length.error();
            }
        }
        return vec;
    }

    boost::outcome_v2::result<std::vector<double>> Result::as_double_vector() const noexcept
    {
        if (type() != ResultType::Array)
        {
            return boost::system::errc::invalid_argument;
        }
        auto it = val.cbegin() + 1;
        auto length = utils::retrieve_length(it, val.cend());
        if (!length)
        {
            return boost::system::errc::invalid_argument;
        }
        std::vector<double> vec;
        vec.reserve(length.value());
        for (std::int64_t i = 0; i < length.value(); i++)
        {
            auto length = utils::next_item_length(it, val.cend());
            if (length && length.value() > 2)
            {
                switch (*it)
                {
                case '+':
                case ':':
                {
                    double result;
                    [[maybe_unused]] auto [ignore, ec] = std::from_chars(it.base() + 1, it.base() + length.value() - 2, result);
                    if (ec != std::errc{})
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    vec.push_back(result);
                    break;
                }
                case '$':
                {
                    auto parse_it = it + 1;
                    auto length = utils::retrieve_length(parse_it, val.cend());
                    if (length && length.value() > 0 && std::distance(parse_it, val.cend()) >= length.value() + 2)
                    {
                        double result;
                        [[maybe_unused]] auto [ignore, ec] = std::from_chars(parse_it.base(), parse_it.base() + length.value(), result);
                        if (ec != std::errc{})
                        {
                            return boost::system::errc::invalid_argument;
                        }
                        vec.push_back(result);
                    }
                    else
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    break;
                }
                case '-':
                case '*':
                    return boost::system::errc::invalid_argument;
                }
                it += length.value();
            }
            else
            {
                return length.error();
            }
        }
        return vec;
    }

    boost::outcome_v2::result<std::vector<bool>> Result::as_boolean_vector() const noexcept
    {
        if (type() != ResultType::Array)
        {
            return boost::system::errc::invalid_argument;
        }
        auto it = val.cbegin() + 1;
        auto length = utils::retrieve_length(it, val.cend());
        if (!length)
        {
            return boost::system::errc::invalid_argument;
        }
        std::vector<bool> vec;
        vec.reserve(length.value());
        for (std::int64_t i = 0; i < length.value(); i++)
        {
            auto length = utils::next_item_length(it, val.cend());
            if (length && length.value() > 2)
            {
                switch (*it)
                {
                case '+':
                case ':':
                {
                    if (utils::case_insensitive_equal(std::string_view{it + 1, it + length.value()}, "true\r\n") || utils::case_insensitive_equal(std::string_view{it + 1, it + length.value()}, "1\r\n") || utils::case_insensitive_equal(std::string_view{it + 1, it + length.value()}, "t\r\n"))
                    {
                        vec.push_back(true);
                        break;
                    }
                    vec.push_back(false);
                    break;
                }
                case '$':
                {
                    auto parse_it = it + 1;
                    auto length = utils::retrieve_length(parse_it, val.cend());
                    if (length && length.value() > 0 && std::distance(parse_it, val.cend()) >= length.value() + 2)
                    {
                        if (utils::case_insensitive_equal(std::string_view{parse_it, parse_it + length.value()}, "true") || utils::case_insensitive_equal(std::string_view{parse_it, parse_it + length.value()}, "1") || utils::case_insensitive_equal(std::string_view{parse_it, parse_it + length.value()}, "t"))
                        {
                            vec.push_back(true);
                            break;
                        }
                        vec.push_back(false);
                    }
                    else
                    {
                        return boost::system::errc::invalid_argument;
                    }
                    break;
                }
                case '-':
                case '*':
                    return boost::system::errc::invalid_argument;
                }
                it += length.value();
            }
            else
            {
                return length.error();
            }
        }
        return vec;
    }

    std::string Result::as_string(std::string default_value) const noexcept
    {
        auto v = as_string();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::int32_t Result::as_int32(std::int32_t default_value) const noexcept
    {
        auto v = as_int32();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::int64_t Result::as_int64(std::int64_t default_value) const noexcept
    {
        auto v = as_int64();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::uint32_t Result::as_uint32(std::uint32_t default_value) const noexcept
    {
        auto v = as_uint32();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::uint64_t Result::as_uint64(std::uint64_t default_value) const noexcept
    {
        auto v = as_uint64();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    bool Result::as_boolean(bool default_value) const noexcept
    {
        auto v = as_boolean();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    float Result::as_float(float default_value) const noexcept
    {
        auto v = as_float();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    double Result::as_double(double default_value) const noexcept
    {
        auto v = as_double();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::vector<Result> Result::as_vector(std::vector<Result> default_value) const noexcept
    {
        auto v = as_vector();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::vector<std::string> Result::as_string_vector(std::vector<std::string> default_value) const noexcept
    {
        auto v = as_string_vector();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::vector<std::int64_t> Result::as_int64_vector(std::vector<std::int64_t> default_value) const noexcept
    {
        auto v = as_int64_vector();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::vector<std::uint64_t> Result::as_uint64_vector(std::vector<std::uint64_t> default_value) const noexcept
    {
        auto v = as_uint64_vector();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::vector<float> Result::as_float_vector(std::vector<float> default_value) const noexcept
    {
        auto v = as_float_vector();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::vector<double> Result::as_double_vector(std::vector<double> default_value) const noexcept
    {
        auto v = as_double_vector();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    std::vector<bool> Result::as_boolean_vector(std::vector<bool> default_value) const noexcept
    {
        auto v = as_boolean_vector();
        if (v)
        {
            return v.value();
        }
        return default_value;
    }

    boost::asio::awaitable<boost::outcome_v2::result<IntResult>> IntResult::create_from_connection(network::Connection &conn){
        std::string buf_container;
        boost::asio::dynamic_string_buffer buf(buf_container);
        auto result = co_await conn.read_until(buf, "\r\n");
        if(result.has_error()){
            co_return result.error();
        }
    }

} // namespace reddish::resp
