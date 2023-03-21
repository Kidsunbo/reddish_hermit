#include "reddish/resp/reader.h"
#include "reddish/utils/string.h"

namespace reddish::resp
{
    Reader::Reader() : buf_string{}, index_offset(0), buf(buf_string){}

    AsyncResult<std::string> Reader::read_whole_reply(network::Connection &conn)
    {
        if (buf_string.capacity() < 20)
            buf_string.reserve(30);
        
        index_offset = 0;

        auto reply_length = co_await conn.read_exact(buf, 1);
        if (reply_length.has_error())
        {
            co_return reply_length.error();
        }
        if (buf_string.length() == 0) [[unlikely]]
        {
            co_return boost::system::errc::invalid_argument;
        }

        switch (buf_string[0])
        {
        case '+':
        case '-':
        case ':':
        {
            auto res = co_await read_whole_simple(conn);
            if (res.has_error())
            {
                co_return res.error();
            }
            if (static_cast<std::int64_t>(buf_string.length()) < res.value())
            {
                co_return boost::system::errc::invalid_argument;
            }
            auto result = buf_string.substr(0, res.value());
            buf.consume(result.size());
            co_return result;
        }
        case '$':
        {
            auto res = co_await read_whole_bulk_string(conn);
            if (res.has_error())
            {
                co_return res.error();
            }
            if (static_cast<std::int64_t>(buf_string.length()) < res.value())
            {
                co_return boost::system::errc::invalid_argument;
            }
            auto result = buf_string.substr(0, res.value());
            buf.consume(result.size());
            co_return result;
        }
        case '*':
        {
            auto res = co_await read_whole_array(conn);
            if (res.has_error())
            {
                co_return res.error();
            }
            if (static_cast<std::int64_t>(buf_string.length()) < res.value())
            {
                co_return boost::system::errc::invalid_argument;
            }
            auto result = buf_string.substr(0, res.value());
            buf.consume(result.size());
            co_return result;
        }
        default:
            co_return boost::system::errc::invalid_argument;
        }
    }

    AsyncResult<std::int64_t> Reader::read_whole_bulk_string(network::Connection &conn)
    {
        auto result = co_await conn.read_until(buf, "\r\n");
        if(result.has_error()){
            co_return result.error();
        }
        auto it = buf_string.cbegin() + index_offset;
        it++; // skip the leading '$'
        auto length = utils::retrieve_length(it, buf_string.cend());
        if(length.has_error()){
            co_return length.error();
        }
        if(length.value() > 0){
            auto content = co_await conn.read_exact(buf, length.value()+2);
            if(content.has_error()){
                co_return content.error();
            }
            index_offset += result.value() + content.value();
            co_return result.value() + content.value();
        }else{
            auto content = co_await conn.read_exact(buf, 2);
            if(content.has_error()){
                co_return content.error();
            }
            index_offset += result.value() + content.value();
            co_return result.value() + content.value();
        }
    }

    AsyncResult<std::int64_t> Reader::read_whole_array(network::Connection &conn)
    {
        static_cast<void>(conn);
        co_return 0;
    }

    AsyncResult<std::int64_t> Reader::read_whole_simple(network::Connection &conn)
    {
        auto length = co_await conn.read_until(buf, "\r\n");
        if(length.has_error()){
            co_return length.error();
        }
        index_offset += length.value();
        co_return length.value();
    }

} // namespace reddish::resp
