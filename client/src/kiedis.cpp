#include "reddish/kiedis.h"
#include "common/command/command.h"


namespace reddish {
    common::utils::AsyncResult<common::protocol::RESPValue> KiedisClient::get(std::string_view key){
        auto command = common::commands::Command<common::commands::CommandEnum::Get>::to_string(key);

        auto written = co_await conn.write(command);
        if (!written) {
            co_return written.error();
        }

        common::protocol::RESPReader reader(conn);
        auto reply = co_await reader.read();
        if (!reply) {
            co_return reply.error();
        }

        co_return reply.value();
    }

}
