#include "reddish/proto/proto.h"

namespace reddish::proto
{

    std::string proto_wrap(std::string_view value){
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


    std::string proto_wrap(double value){
        return proto_wrap(std::to_string(value));
    }
    std::string proto_wrap(std::int64_t value){
        return proto_wrap(std::to_string(value));
    }
    std::string proto_wrap(bool value){
        return proto_wrap(std::to_string(value));
    }

    std::string proto_wrap(const char* value){
        return proto_wrap(std::string_view{value});
    }

} // namespace reddish::proto
