#ifndef REDDISH_HERMIT_UTILS_STR_H
#define REDDISH_HERMIT_UTILS_STR_H

#include <cstddef>
#include <string>

#include <boost/outcome.hpp>

namespace reddish::utils
{
    bool case_insensitive_equal(char a, char b);

    bool case_insensitive_equal(std::string_view a, std::string_view b);

    boost::outcome_v2::result<std::int64_t> retrieve_length(std::string::const_iterator& it, const std::string::const_iterator& end);

    boost::outcome_v2::result<std::string> retrieve_string_without_advance(std::string::const_iterator& it, const std::string::const_iterator& end, std::int64_t length);

    bool advance_if_same(std::string::const_iterator& it, const std::string::const_iterator& end, std::string_view s);

    std::size_t advance_to_next_line(std::string::const_iterator& it, const std::string::const_iterator& end, std::string_view s);

    boost::outcome_v2::result<std::int64_t> next_item_length(std::string::const_iterator it, std::string::const_iterator end);

} // namespace reddish::utils



#endif