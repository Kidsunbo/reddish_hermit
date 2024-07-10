#pragma once

#include <cstdint>
#include <string>
#include <variant>
namespace reddish::common::protocol {

    // the data type for redis protocol, for now, only RESP2 is supported
    enum class RESPDataType : char {
        Error = '-',
        SimpleString = '+',
        Integer = ':',
        BulkString = '$',
        Arrays = '*',
    };


    using KiedisSupportType = std::variant<std::int64_t, bool, std::string>;

}