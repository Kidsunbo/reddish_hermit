#include "reddish/resp/request.h"


namespace reddish::resp
{
    std::string Request::wrap_item(std::string_view value)
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

    std::string Request::wrap_item(float value)
    {
        std::stringstream ss;
        ss << value;
        return wrap_item(ss.str());
    }

    std::string Request::wrap_item(double value)
    {
        std::stringstream ss;
        ss << value;
        return wrap_item(ss.str());
    }

    std::string Request::wrap_item(std::int64_t value)
    {
        return wrap_item(std::to_string(value));
    }

    std::string Request::wrap_item(std::uint64_t value)
    {
        return wrap_item(std::to_string(value));
    }

    std::string Request::wrap_item(std::int32_t value)
    {
        return wrap_item(std::to_string(value));
    }

    std::string Request::wrap_item(std::uint32_t value)
    {
        return wrap_item(std::to_string(value));
    }

    std::string Request::wrap_item(bool value)
    {
        return value ? wrap_item("true") : wrap_item("false");
    }

    std::string Request::wrap_item(const char *value)
    {
        return wrap_item(std::string_view{value});
    }

    std::string Request::wrap_item(long double value)
    {
        std::stringstream ss;
        ss << value;
        return wrap_item(ss.str());
    }

    std::string_view Request::to_string_view()
    {
        return request_str;
    }

    std::string Request::to_string()
    {
        return request_str;
    }
} // namespace reddish::resp
