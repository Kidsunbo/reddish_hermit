#ifndef REDDISH_HERMIT_PROTO_PROTO_H
#define REDDISH_HERMIT_PROTO_PROTO_H

#include <string>
#include "command.h"

namespace reddish::proto
{

    std::string proto_wrap(std::string_view value);
    std::string proto_wrap(const char* value);
    std::string proto_wrap(double value);
    std::string proto_wrap(std::int64_t value);
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