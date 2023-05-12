#include "reddish/resp/reader.h"
#include "reddish/utils/string.h"

namespace reddish::resp
{
    OnceReader::OnceReader() : result{}, buf_string{}, buf(buf_string){}

    AsyncResult<std::string> OnceReader::read_whole_reply(network::Connection &conn)
    {
        buf_string.reserve(30);
        result.reserve(30);
        
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
            buf.consume(1);
            auto res = co_await read_whole_simple(conn);
            if (res.has_error())
            {
                co_return res.error();
            }
            break;
        }
        case '$':
        {
            buf.consume(1);
            auto res = co_await read_whole_bulk_string(conn);
            if (res.has_error())
            {
                co_return res.error();
            }
            break;
        }
        case '*':
        {
            buf.consume(1);
            auto res = co_await read_whole_array(conn);
            if (res.has_error())
            {
                co_return res.error();
            }
            break;
        }
        default:
            co_return boost::system::errc::invalid_argument;
        }
        co_return result;
    }

    AsyncResult<void> OnceReader::read_whole_bulk_string(network::Connection &conn)
    {
        auto length_of_length = co_await conn.read_until(buf, "\r\n");
        if(length_of_length.has_error()){
            co_return length_of_length.error();
        }
        auto it = buf_string.cbegin();
        auto length = utils::retrieve_length(it, buf_string.cend());
        if(length.has_error()){
            co_return length.error();
        }
        buf.consume(length_of_length.value());
        if(length.value() > 0){
            auto content = co_await conn.read_exact(buf, length.value()+2);
            if(content.has_error()){
                co_return content.error();
            }
            result = std::move(buf_string);
            result.pop_back();
            result.pop_back();
            co_return boost::outcome_v2::success();
        }else{
            auto content = co_await conn.read_exact(buf, 2);
            if(content.has_error()){
                co_return content.error();
            }
            co_return boost::outcome_v2::success();
        }
    }

    AsyncResult<void> OnceReader::read_whole_array(network::Connection &conn)
    {
        static_cast<void>(conn);
        co_return boost::outcome_v2::success();
    }

    AsyncResult<void> OnceReader::read_whole_simple(network::Connection &conn)
    {
        auto length = co_await conn.read_until(buf, "\r\n");
        if(length.has_error()){
            co_return length.error();
        }
        result = std::move(buf_string);
        result.pop_back();
        result.pop_back();
        co_return boost::outcome_v2::success();
    }

} // namespace reddish::resp
