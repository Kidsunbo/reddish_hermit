#include "reddish/utils/string.h"

namespace reddish::utils
{

    bool case_insensitive_equal(char a, char b)
    {
        if ((a > 96) && (a < 123))
            a ^= 0x20;
        if ((b > 96) && (b < 123))
            b ^= 0x20;
        return a == b;
    }

    bool case_insensitive_equal(std::string_view a, std::string_view b)
    {
        if (a.length() != b.length())
        {
            return false;
        }
        for (std::size_t i = 0; i < a.length(); i++)
        {
            if (!case_insensitive_equal(a[i], b[i]))
            {
                return false;
            }
        }
        return true;
    }

    SyncResult<std::int64_t> retrieve_length(std::string::const_iterator &it, const std::string::const_iterator &end)
    {
        std::int64_t length = 0;
        bool is_first = true;
        bool is_positive = true;
        for (; it != end; it++)
        {
            if (is_first)
            {
                is_first = false;
                if (*it == '+') [[unlikely]]
                {
                    is_positive = true;
                    continue;
                }
                if (*it == '-')
                {
                    is_positive = false;
                    continue;
                }
            }
            if (*it < '0' || *it > '9')
            {
                break;
            }
            length = length * 10 + *it - '0';
        }
        if (!advance_if_same(it, end, "\r\n"))
        {
            return boost::system::errc::invalid_argument;
        }
        return is_positive ? length : -length;
    }

    SyncResult<std::string> retrieve_string_without_advance(std::string::const_iterator &it, const std::string::const_iterator &end, std::int64_t length)
    {
        if (length >= 0 && std::distance(it, end) >= length)
        {
            return std::string{it, it + length};
        }
        else if (length == -1)
        {
            return "nil";
        }
        else
        {
            return boost::system::errc::invalid_argument;
        }
    }

    bool advance_if_same(std::string::const_iterator &it, const std::string::const_iterator &end, std::string_view s)
    {
        if (std::distance(it, end) < static_cast<std::ptrdiff_t>(s.length()))
        {
            return false;
        }
        for (auto s_it = s.cbegin(); it != end && s_it != s.cend(); it++, s_it++)
        {
            if (*it != *s_it)
            {
                return false;
            }
        }
        return true;
    }

    std::size_t advance_to_next_line(std::string::const_iterator &it, const std::string::const_iterator &end)
    {
        std::size_t steps = 0;
        while (std::distance(it, end) > 1)
        {
            if (*it == '\r' && *(it + 1) == '\n')
            {
                it += 2;
                steps += 2;
                break;
            }
            it++;
            steps++;
        }
        return steps;
}

    SyncResult<std::int64_t> next_item_length(std::string::const_iterator it, std::string::const_iterator end)
    {
        if(it == end){
            return boost::system::errc::invalid_argument;
        }
        switch (*it)
        {
        case '+':
        case '-':
        case ':':
        {
            return utils::advance_to_next_line(it, end);
            break;
        }
        case '$':
        {
            auto item_start = it;
            auto length = utils::retrieve_length(++it, end);
            if (length && length.value() == -1)
            {
                return std::distance(item_start, it);
            }
            else if (length && length.value() > 0 && std::distance(it, end) >= length.value() + 2)
            {
                return std::distance(item_start, it) + length.value() + 2;
            }
            else
            {
                return boost::system::errc::invalid_argument;
            }
            break;
        }
        case '*':
        {
            auto item_start = it;
            auto length = utils::retrieve_length(++it, end);
            if (!length)
            {
                return length.error();
            }
            std::size_t arr_len = std::distance(item_start, it);
            for (std::int64_t i = 0; i < length.value(); i++)
            {
                auto item_len = next_item_length(it, end);
                if (!item_len)
                {
                    return item_len.error();
                }
                arr_len += item_len.value();
                it += item_len.value();
            }
            return arr_len;
        }
        default:
            return boost::system::errc::invalid_argument;
        }
    }

} // namespace reddish::utils
