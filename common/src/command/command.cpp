#include <common/command/command.h>

namespace reddish::common::commands {

    bool equal(Commands, std::string_view)
    {
        return true;
    }

    std::string_view to_string(Commands)
    {
        return "";
    }

}