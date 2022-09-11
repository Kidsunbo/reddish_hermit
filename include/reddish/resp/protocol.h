#ifndef REDDISH_HERMIT_RESP_PROTOCOL_H
#define REDDISH_HERMIT_RESP_PROTOCOL_H

#include <string>
#include <map>
#include <set>
#include "command.h"
#include "../network/connection.h"

#include "boost/outcome.hpp"

namespace reddish::resp
{

    class Request
    {
        std::string request_str;

    private:
        // wrap string and string_view type
        std::string item_wrap(std::string_view value);
        // wrap const char* type
        std::string item_wrap(const char *value);
        // wrap float type
        std::string item_wrap(float value);
        // wrap double type
        std::string item_wrap(double value);
        // wrap long double type
        std::string item_wrap(long double value);
        // wrap short, unsigned short and int type
        std::string item_wrap(std::int32_t value);
        // wrap unsigned int type
        std::string item_wrap(std::uint32_t value);
        // wrap long(64bit), long long type
        std::string item_wrap(std::int64_t value);
        // wrap unsigned long(64bit), unsigned long long(32bit) type
        std::string item_wrap(std::uint64_t value);
        // wrap boolean type
        std::string item_wrap(bool value);

        template <std::size_t SizeHint = 30, typename... ARGS>
        std::string to_request_str(Command cmd, ARGS &&...args)
        {
            std::string buf;
            buf.reserve(SizeHint);
            const std::size_t arr_size = 1 + sizeof...(args);
            buf.append("*");
            buf.append(std::to_string(arr_size));
            buf.append("\r\n");

            buf.append(item_wrap(resp::to_string(cmd)));
            (buf.append(item_wrap(args)), ...);
            return buf;
        }

    public:
        template <typename... ARGS>
        Request(Command cmd, ARGS &&...args) : request_str(to_request_str(cmd, std::forward<ARGS>(args)...)) {}

        std::string_view to_string_view();

        std::string to_string();
    };

    class Result
    {
        enum class ResultType
        {
            SimpleString,
            Error,
            Integer,
            BulkString,
            Array,
        };

        std::string val;
        ResultType val_type;

    public:
        boost::outcome_v2::result<Result> create_from_connection(network::Connection &conn);
        ResultType type();

        boost::outcome_v2::result<std::string> as_string();

        boost::outcome_v2::result<std::int32_t> as_int32();

        boost::outcome_v2::result<std::int64_t> as_int64();

        boost::outcome_v2::result<std::uint32_t> as_uint32();

        boost::outcome_v2::result<std::uint64_t> as_uint64();

        boost::outcome_v2::result<bool> as_boolean();

        boost::outcome_v2::result<float> as_float();

        boost::outcome_v2::result<double> as_double();

        boost::outcome_v2::result<std::vector<Result>> as_vector();

        boost::outcome_v2::result<std::vector<std::string>> as_string_vector();

        boost::outcome_v2::result<std::vector<std::int64_t>> as_int64_vector();

        boost::outcome_v2::result<std::vector<std::uint64_t>> as_uint64_vector();

        boost::outcome_v2::result<std::vector<float>> as_float_vector();

        boost::outcome_v2::result<std::vector<double>> as_double_vector();

        boost::outcome_v2::result<std::vector<bool>> as_boolean_vector();

        std::string as_string(std::string default_value);

        std::int32_t as_int32(std::int32_t default_value);

        std::int64_t as_int64(std::int64_t default_value);

        std::uint32_t as_uint32(std::uint32_t default_value);

        std::uint64_t as_uint64(std::uint64_t default_value);

        bool as_boolean(bool default_value);

        float as_float(float default_value);

        double as_double(double default_value);

        std::vector<Result> as_vector(std::vector<Result> default_value);

        std::vector<std::string> as_string_vector(std::vector<std::string> default_value);

        std::vector<std::int64_t> as_int64_vector(std::vector<std::int64_t> default_value);

        std::vector<std::uint64_t> as_uint64_vector(std::vector<std::uint64_t> default_value);

        std::vector<float> as_float_vector(std::vector<float> default_value);

        std::vector<double> as_double_vector(std::vector<double> default_value);

        std::vector<bool> as_boolean_vector(std::vector<bool> default_value);
    };

    template <typename T>
    concept specific_result_concept = requires(T t, network::Connection conn)
    {
        typename T::value_type;
        {T::create_from_connection(conn)} -> std::same_as<boost::outcome_v2::result<T>>;
        {t.result()} -> std::same_as<boost::outcome_v2::result<typename T::value_type>&>;
        {t.result(std::declval<typename T::value_type>())} -> std::same_as<typename T::value_type>;
    };

    class IntResult
    {

    public:
        using value_type = std::int64_t;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<IntResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<IntResult>, "IntResult does not satisfy the requirement");


    class StringResult
    {

    public:
        using value_type = std::string;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<StringResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<StringResult>, "StringResult does not satisfy the requirement");

    class BoolResult
    {
    public:
        using value_type = bool;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<BoolResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<BoolResult>, "BoolResult does not satisfy the requirement");

    class VectorResult
    {
    public:
        using value_type = std::vector<Result>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<VectorResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);

    };
    static_assert(specific_result_concept<VectorResult>, "VectorResult does not satisfy the requirement");

    class DoubleResult
    {
    public:
        using value_type = double;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<DoubleResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<DoubleResult>, "DoubleResult does not satisfy the requirement");

    class DoubleVectorResult
    {
    public:
        using value_type = std::vector<double>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<DoubleVectorResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<DoubleVectorResult>, "DoubleResult does not satisfy the requirement");

    class IntVectorResult
    {
    public:
        using value_type = std::vector<std::int64_t>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<IntVectorResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<IntVectorResult>, "IntVectorResult does not satisfy the requirement");

    class StringVectorResult
    {
    public:
        using value_type = std::vector<std::string>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<StringVectorResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<StringVectorResult>, "StringVectorResult does not satisfy the requirement");

    class BoolVectorResult
    {
    public:
        using value_type = std::vector<bool>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:
        static boost::outcome_v2::result<BoolVectorResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<BoolVectorResult>, "BoolVectorResult does not satisfy the requirement");

    class MapResult
    {
    public:
        using value_type = std::map<std::string, Result>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:

        static boost::outcome_v2::result<MapResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<MapResult>, "MapResult does not satisfy the requirement");

    class MapStringStringResult
    {
    public:
        using value_type = std::map<std::string, std::string>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:

        static boost::outcome_v2::result<MapStringStringResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<MapStringStringResult>, "MapStringStringResult does not satisfy the requirement");

    class MapStringIntResult
    {
    public:
        using value_type = std::map<std::string, std::int64_t>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:

        static boost::outcome_v2::result<MapStringIntResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<MapStringIntResult>, "MapStringIntResult does not satisfy the requirement");

    class SetStringResult
    {
    public:
        using value_type = std::set<std::string>;

    private:
        boost::outcome_v2::result<value_type> val;

    public:

        static boost::outcome_v2::result<SetStringResult> create_from_connection(network::Connection &conn);

        boost::outcome_v2::result<value_type> &result();

        value_type result(value_type defalt_value);
    };
    static_assert(specific_result_concept<SetStringResult>, "SetStringResult does not satisfy the requirement");


} // namespace reddish::resp

#endif