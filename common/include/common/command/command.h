#pragma once

#include <string_view>

#define __Reddish_RegisterCommands(x) constexpr std::string_view x = #x;

namespace reddish::common::commands {
    // G
    __Reddish_RegisterCommands(GET)
    __Reddish_RegisterCommands(GETBIT)
    __Reddish_RegisterCommands(GETDEL)
    __Reddish_RegisterCommands(GETEX)
    __Reddish_RegisterCommands(GETRANGE)
    __Reddish_RegisterCommands(GETSET)

    // S
    __Reddish_RegisterCommands(SET)
    __Reddish_RegisterCommands(SETBIT)
    __Reddish_RegisterCommands(SETEX)
    __Reddish_RegisterCommands(SETNX)
    __Reddish_RegisterCommands(SETRANGE)

}
