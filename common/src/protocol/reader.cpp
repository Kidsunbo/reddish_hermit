#include <boost/system/errc.hpp>
#include <common/protocol/reader.h>
#include <common/protocol/resp_value.h>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace reddish::common::protocol {

    utils::AsyncResult<std::string> RESPReader::read_line()
    {
        co_return co_await conn.read_until("\r\n");
    }

    utils::AsyncResult<std::string> RESPReader::read_exact(std::uint64_t size)
    {
        co_return co_await conn.read_exact(size);
    }

    utils::AsyncResult<RESPValue> RESPReader::read()
    {
        auto line = co_await read_line();
        if (!line) {
            co_return line.error();
        }

        const std::string& text = line.value();
        if (text.size() < 3 || text[text.size() - 2] != '\r' || text[text.size() - 1] != '\n') {
            co_return boost::system::errc::protocol_error;
        }

        const auto type = resp2_type_from_prefix(text[0]);
        if (!type) {
            co_return boost::system::errc::protocol_error;
        }

        switch (*type) {
        case RESP2Type::SimpleString: {
            auto parsed = parse_simple_string(text);
            if (!parsed) {
                co_return parsed.error();
            }
            co_return parsed.value();
        }
        case RESP2Type::Error: {
            auto parsed = parse_error(text);
            if (!parsed) {
                co_return parsed.error();
            }
            co_return parsed.value();
        }
        case RESP2Type::Integer: {
            auto parsed = parse_integer(text);
            if (!parsed) {
                co_return parsed.error();
            }
            co_return parsed.value();
        }
        case RESP2Type::BulkString: {
            auto parsed = parse_length(text);
            if (!parsed) {
                co_return parsed.error();
            }
            const std::int64_t length = parsed.value();
            if (length == -1) {
                co_return RESPValue::null();
            }
            auto payload = co_await read_exact(static_cast<std::uint64_t>(length) + 2);
            if (!payload) {
                co_return payload.error();
            }
            const std::string& bytes = payload.value();
            if (bytes.size() < static_cast<std::size_t>(length) + 2 || bytes[bytes.size() - 2] != '\r'
                || bytes[bytes.size() - 1] != '\n') {
                co_return boost::system::errc::protocol_error;
            }
            co_return RESPValue::bulk_string(bytes.substr(0, static_cast<std::size_t>(length)));
        }
        case RESP2Type::Array: {
            auto parsed = parse_length(text);
            if (!parsed) {
                co_return parsed.error();
            }
            const std::int64_t length = parsed.value();
            if (length == -1) {
                co_return RESPValue::null();
            }
            std::vector<RESPValue> items;
            items.reserve(static_cast<std::size_t>(length));
            for (std::int64_t i = 0; i < length; ++i) {
                auto item = co_await read();
                if (!item) {
                    co_return item.error();
                }
                items.push_back(std::move(item).value());
            }
            co_return RESPValue::array(std::move(items));
        }
        default:
            co_return boost::system::errc::protocol_error;
        }
    }

}
