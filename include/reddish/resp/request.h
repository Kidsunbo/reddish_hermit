#ifndef REDDISH_HERMIT_RESP_REQUEST_H
#define REDDISH_HERMIT_RESP_REQUEST_H

#include <string>
#include <map>
#include <set>
#include "command.h"
#include "reddish/network/connection.h"
#include "reddish/common/return_type.h"

#include <boost/outcome.hpp>

namespace reddish::resp
{
    class Request
    {
        std::string request_str;

    private:
        // wrap string and string_view type
        std::string item_wrap(std::string_view value);
        // wrap const char* type
        std::string item_wrap(const char *value);
        // wrap float type
        std::string item_wrap(float value);
        // wrap double type
        std::string item_wrap(double value);
        // wrap long double type
        std::string item_wrap(long double value);
        // wrap short, unsigned short and int type
        std::string item_wrap(std::int32_t value);
        // wrap unsigned int type
        std::string item_wrap(std::uint32_t value);
        // wrap long(64bit), long long type
        std::string item_wrap(std::int64_t value);
        // wrap unsigned long(64bit), unsigned long long(32bit) type
        std::string item_wrap(std::uint64_t value);
        // wrap boolean type
        std::string item_wrap(bool value);

        template <std::size_t SizeHint = 30, typename... ARGS>
        std::string to_request_string(Command cmd, ARGS &&...args)
        {
            std::string buf;
            buf.reserve(SizeHint);
            const std::size_t arr_size = 1 + sizeof...(args);
            buf.append("*");
            buf.append(std::to_string(arr_size));
            buf.append("\r\n");

            buf.append(item_wrap(resp::to_string(cmd)));
            (buf.append(item_wrap(args)), ...);
            return buf;
        }

    public:
        template <typename... ARGS>
        Request(Command cmd, ARGS &&...args) : request_str(to_request_string(cmd, std::forward<ARGS>(args)...)) {}

        std::string_view to_string_view();

        std::string to_string();

    };

}

#endif