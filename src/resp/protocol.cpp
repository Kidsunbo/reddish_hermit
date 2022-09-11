#include "reddish/resp/protocol.h"

#include <sstream>
namespace reddish::resp
{

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


    std::string_view Request::to_string_view(){
        return request_str;
    }

    std::string Request::to_string(){
        return request_str;
    }

} // namespace reddish::resp
