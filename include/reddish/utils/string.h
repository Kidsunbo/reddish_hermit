#ifndef REDDISH_HERMIT_UTILS_STRING_H
#define REDDISH_HERMIT_UTILS_STRING_H

#include <cstddef>
#include <string>

#include <boost/outcome.hpp>
#include "reddish/common/return_type.h"

namespace reddish::utils
{
    bool case_insensitive_equal(char a, char b);

    bool case_insensitive_equal(std::string_view a, std::string_view b);

    SyncResult<std::int64_t> retrieve_length(std::string::const_iterator& it, const std::string::const_iterator& end);

    SyncResult<std::string> retrieve_string_without_advance(std::string::const_iterator& it, const std::string::const_iterator& end, std::int64_t length);

    bool advance_if_same(std::string::const_iterator& it, const std::string::const_iterator& end, std::string_view s);

    std::size_t advance_to_next_line(std::string::const_iterator& it, const std::string::const_iterator& end);

    SyncResult<std::int64_t> next_item_length(std::string::const_iterator it, std::string::const_iterator end);

    /*
    Parse the string to int64 when possible.
    @param it The iterator of the string.
    @param end The end of the string.
    @param try_to If this is true, it won't produce an error and try to return the integer with as many characters as possible.
                If this is false, it will produce error as normal.
    @param until_new_line If try_to is true, and until_new_line is true,

    */
    SyncResult<std::int64_t> to_int64(std::string::const_iterator it, std::string::const_iterator end, bool try_to = true, bool until_new_line = true);

} // namespace reddish::utils



#endif