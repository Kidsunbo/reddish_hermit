#include <common/protocol/protocol.h>
#include <common/protocol/resp_value.h>
#include <common/protocol/writer.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace reddish::common::protocol {

    utils::AsyncResult<std::size_t> RESPWriter::write(const RESPValue& value)
    {
        co_return co_await conn.write(encode(value));
    }

    utils::AsyncResult<std::size_t> RESPWriter::write_simple_string(std::string value)
    {
        co_return co_await write(RESPValue::simple_string(std::move(value)));
    }

    utils::AsyncResult<std::size_t> RESPWriter::write_bulk_string(std::string value)
    {
        co_return co_await write(RESPValue::bulk_string(std::move(value)));
    }

    utils::AsyncResult<std::size_t> RESPWriter::write_null_bulk_string()
    {
        co_return co_await write(RESPValue::null());
    }

    utils::AsyncResult<std::size_t> RESPWriter::write_array(std::vector<std::string> value)
    {
        std::vector<RESPValue> items;
        items.reserve(value.size());
        for (auto& item : value) {
            items.push_back(RESPValue::bulk_string(std::move(item)));
        }
        co_return co_await write(RESPValue::array(std::move(items)));
    }

    utils::AsyncResult<std::size_t> RESPWriter::write_error(std::string code, std::string message)
    {
        co_return co_await write(RESPValue::error(std::move(code), std::move(message)));
    }

    utils::AsyncResult<std::size_t> RESPWriter::write_integer(std::int64_t value)
    {
        co_return co_await write(RESPValue::integer(value));
    }

}
