#ifndef REDDISH_HERMIT_RESP_COMMAND_H
#define REDDISH_HERMIT_RESP_COMMAND_H

#include <string_view>

namespace reddish::resp
{
    enum class Command
    {
        //String Command
        Append,
        Decrease,
        DecreaseBy,
        Get,
        GetDel,
        GetEX,
        GetRange,
        GetSet,
        Increase,
        IncreaseBy,
        IncreaseByFloat,
        LongestCommonString,
        MGet,
        MSet,
        MSetNX,
        PSetEX,
        Set,
        SetEX,
        SetNX,
        SetRange,
        StrLen,
        SubStr,
    };

    constexpr std::string_view to_string(const Command cmd)
    {
        switch (cmd)
        {
        case Command::Append:
        return "APPEND";
        case Command::Decrease:
        return "DECR";
        case Command::DecreaseBy:
        return "DECRBY";
        case Command::Get:
        return "GET";
        case Command::GetDel:
        return "GETDEL";
        case Command::GetEX:
        return "GETEX";
        case Command::GetRange:
        return "GETRANGE";
        case Command::GetSet:
        return "GETSET";
        case Command::Increase:
        return "INCR";
        case Command::IncreaseBy:
        return "INCRBY";
        case Command::IncreaseByFloat:
        return "INCRBYFLOAT";
        case Command::LongestCommonString:
        return "LCS";
        case Command::MGet:
        return "MGET";
        case Command::MSet:
        return "MSET";
        case Command::MSetNX:
        return "MSETNX";
        case Command::PSetEX:
        return "PSETEX";
        case Command::Set:
        return "SET";
        case Command::SetEX:
        return "SETEX";
        case Command::SetNX:
        return "SETNX";
        case Command::SetRange:
        return "SETRANGE";
        case Command::StrLen:
        return "STRLEN";
        case Command::SubStr:
        return "SUBSTR";
        default:
            return "UNKNOWN";
            break;
        }
    }

} // namespace reddish::resp

#endif