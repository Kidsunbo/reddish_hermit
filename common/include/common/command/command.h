#pragma once

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

namespace reddish::common::commands {

    template <typename T>
    concept HasTextMethod = requires(T t) {
        { T::text() } -> std::convertible_to<std::string_view>;
    };

    template <typename T>
    concept IsString = std::is_same_v<std::remove_reference_t<T>, std::string_view> || std::is_same_v<std::remove_reference_t<T>, std::string>;

    enum class CommandEnum {
        Get,
        GetBit,
        Set,
        SetBit,
    };

    template <HasTextMethod Command, std::size_t SizeHint = 30, IsString... ARGS>
    std::string to_request_string(ARGS&&... args)
    {
        std::string buf;
        buf.reserve(SizeHint);
        const std::size_t arr_size = 1 + sizeof...(args);
        buf.append("*");
        buf.append(std::to_string(arr_size));
        buf.append("\r\n");

        buf.append(Command::text());
        (buf.append(args), ...);
        return buf;
    }

    template <CommandEnum Value>
    class Command;

    template <>
    class Command<CommandEnum::Get> {
    public:
        constexpr static std::string_view text() { return "GET"; }
        static std::string to_string(std::string_view key) { return to_request_string<Command<CommandEnum::Get>>(key); }
    };

    template <>
    class Command<CommandEnum::GetBit> {
    public:
        constexpr static std::string_view text() { return "GETBIT"; }
        static std::string to_string(std::string_view key) { return to_request_string<Command<CommandEnum::GetBit>>(key); }
    };

    template <>
    class Command<CommandEnum::Set> {
    public:
        constexpr static std::string_view text() { return "SET"; }
        template<typename... ARGS> requires(sizeof...(ARGS) % 2 == 0)
        static std::string to_string(std::string_view key, std::string_view value, ARGS... args){ return to_request_string<Command<CommandEnum::Set>>(key, value, args...);}
    };

}
