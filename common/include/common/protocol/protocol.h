#pragma once

#include <common/protocol/resp_value.h>
#include <common/utils/types.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace reddish::common::protocol {

    std::string encode(const RESPValue& value);

    utils::SyncResult<RESPValue> parse_simple_string(std::string_view line);

    utils::SyncResult<RESPValue> parse_error(std::string_view line);

    utils::SyncResult<RESPValue> parse_integer(std::string_view line);

    utils::SyncResult<std::int64_t> parse_length(std::string_view line);

    std::optional<RESP2Type> resp2_type_from_prefix(char prefix) noexcept;

}
