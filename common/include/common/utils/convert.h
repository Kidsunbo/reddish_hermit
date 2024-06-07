#pragma once

#include <cstdint>
#include <string_view>


namespace reddish::common::utils {
    
    
    std::int64_t to_int(std::string_view sv);


    double to_double(std::string_view sv);
}
