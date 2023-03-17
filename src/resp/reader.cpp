#include "reddish/resp/reader.h"


namespace reddish::resp
{
    AsyncResult<std::string> Reader::read_whole_reply([[maybe_unused]]network::Connection& conn){
        

        co_return "";
    }
} // namespace reddish::resp
