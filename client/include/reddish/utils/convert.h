#ifndef REDDISH_UTILS_CONVERT_H
#define REDDISH_UTILS_CONVERT_H

#include <cstdint>
#include <string_view>
namespace reddish::utils::convert {
std::int64_t to_int(std::string_view sv);
double to_double(std::string_view sv);

} // namespace reddish::utils::convert

#endif