#pragma once

#include <common/protocol/protocol.h>
#include <common/protocol/resp_value.h>

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

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
        std::vector<protocol::RESPValue> items;
        items.reserve(1 + sizeof...(args));
        items.emplace_back(protocol::RESPValue::bulk_string(std::string(Command::text())));
        (items.emplace_back(protocol::RESPValue::bulk_string(std::string(std::forward<ARGS>(args)))), ...);
        return protocol::encode(protocol::RESPValue::array(std::move(items)));
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
        static std::string to_string(std::string_view key, std::string_view value, ARGS... args){ return to_request_string<Command<CommandEnum::Set>>(key, value, std::to_string(args)...);}
    };

    template <>
    class Command<CommandEnum::SetBit> {
    public:
        constexpr static std::string_view text() { return "SETBIT"; }
        static std::string to_string(std::string_view key, std::uint32_t offset, std::int8_t value){ return to_request_string<Command<CommandEnum::SetBit>>(key, std::to_string(offset), std::to_string(value));}
    };

}
