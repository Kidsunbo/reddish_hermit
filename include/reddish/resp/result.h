#ifndef REDDISH_HERMIT_RESP_RESULT_H
#define REDDISH_HERMIT_RESP_RESULT_H

#include "reddish/common/return_type.h"

namespace reddish::resp
{
    enum class ResultType
    {
        Unknown,
        SimpleString,
        Error,
        Integer,
        BulkString,
        Array,
    };

    class Result
    {
        ResultType type;
    public:
        
    };

} // namespace reddish::resp

#endif