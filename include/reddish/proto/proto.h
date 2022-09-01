#ifndef REDDISH_HERMIT_PROTO_PROTO_H
#define REDDISH_HERMIT_PROTO_PROTO_H

#include <string>
#include "command.h"

namespace reddish::proto
{

    // wrap string and string_view type
    std::string proto_wrap(std::string_view value);
    // wrap const char* type
    std::string proto_wrap(const char* value);
    // wrap float type
    std::string proto_wrap(float value);
    // wrap double type
    std::string proto_wrap(double value);
    // wrap long double type
    std::string proto_wrap(long double value);
    // wrap short, unsigned short and int type
    std::string proto_wrap(std::int32_t value);
    // wrap unsigned int type
    std::string proto_wrap(std::uint32_t value);
    // wrap long(64bit), long long type
    std::string proto_wrap(std::int64_t value);
    // wrap unsigned long(64bit), unsigned long long(32bit) type
    std::string proto_wrap(std::uint64_t value);
    // wrap boolean type
    std::string proto_wrap(bool value);


    template<std::size_t SizeHint = 30, typename... ARGS>
    std::string to_request_str(Command cmd, ARGS&&... args){
        std::string buf;
        buf.reserve(SizeHint);
        const std::size_t arr_size = 1+ sizeof...(args);
        buf.append("*");
        buf.append(std::to_string(arr_size));
        buf.append("\r\n");

        buf.append(proto_wrap(to_string(cmd)));
        (buf.append(proto_wrap(args)),...);
        return buf;
    }
} // namespace reddish::proto



#endif