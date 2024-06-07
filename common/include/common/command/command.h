#pragma once

#include <string_view>

namespace reddish::common::commands {
   
   enum class Commands {
        Get,
        GetBit,
        Set,
        SetBit,
   };


    bool equal(Commands, std::string_view);

    std::string_view to_string(Commands);

}
