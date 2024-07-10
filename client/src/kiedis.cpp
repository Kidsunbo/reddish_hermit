#include "reddish/kiedis.h"
#include "common/command/command.h"


namespace reddish {
    common::utils::AsyncResult<common::protocol::KiedisSupportType> KiedisClient::get(std::string_view key){
        auto commond = common::commands::Command<common::commands::CommandEnum::Get>::to_string(key);


        co_return 0;
    }

}