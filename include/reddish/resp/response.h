#ifndef REDDISH_HERMIT_RESP_RESPONSE_H
#define REDDISH_HERMIT_RESP_RESPONSE_H

#include <string>
#include <map>
#include <set>
#include "command.h"
#include "reddish/network/connection.h"
#include "reddish/common/return_type.h"

#include <boost/outcome.hpp>

namespace reddish::resp
{
    class Result
    {
        std::string val;
    public:
        enum class ResultType
        {
            Unknown,
            SimpleString,
            Error,
            Integer,
            BulkString,
            Array,
        };

        static SyncResult<Result> from_string(const std::string& s);
        static SyncResult<Result> from_string(std::string&& s);

        ResultType type() const noexcept;

        bool is_null() const noexcept;

        SyncResult<std::string> as_string() const noexcept;

        SyncResult<std::int32_t> as_int32() const noexcept;

        SyncResult<std::int64_t> as_int64() const noexcept;

        SyncResult<std::uint32_t> as_uint32() const noexcept;

        SyncResult<std::uint64_t> as_uint64() const noexcept;

        SyncResult<bool> as_boolean() const noexcept;

        SyncResult<float> as_float() const noexcept;

        SyncResult<double> as_double() const noexcept;

        SyncResult<std::vector<Result>> as_vector() const noexcept;

        SyncResult<std::vector<std::string>> as_string_vector() const noexcept;

        SyncResult<std::vector<std::int64_t>> as_int64_vector() const noexcept;

        SyncResult<std::vector<std::uint64_t>> as_uint64_vector() const noexcept;

        SyncResult<std::vector<float>> as_float_vector() const noexcept;

        SyncResult<std::vector<double>> as_double_vector() const noexcept;

        SyncResult<std::vector<bool>> as_boolean_vector() const noexcept;

        std::string as_string(std::string default_value) const noexcept;

        std::int32_t as_int32(std::int32_t default_value) const noexcept;

        std::int64_t as_int64(std::int64_t default_value) const noexcept;

        std::uint32_t as_uint32(std::uint32_t default_value) const noexcept;

        std::uint64_t as_uint64(std::uint64_t default_value) const noexcept;

        bool as_boolean(bool default_value) const noexcept;

        float as_float(float default_value) const noexcept;

        double as_double(double default_value) const noexcept;

        std::vector<Result> as_vector(std::vector<Result> default_value) const noexcept;

        std::vector<std::string> as_string_vector(std::vector<std::string> default_value) const noexcept;

        std::vector<std::int64_t> as_int64_vector(std::vector<std::int64_t> default_value) const noexcept;

        std::vector<std::uint64_t> as_uint64_vector(std::vector<std::uint64_t> default_value) const noexcept;

        std::vector<float> as_float_vector(std::vector<float> default_value) const noexcept;

        std::vector<double> as_double_vector(std::vector<double> default_value) const noexcept;

        std::vector<bool> as_boolean_vector(std::vector<bool> default_value) const noexcept;
    };

    class IntResult
    {

    public:
        using value_type = std::int64_t;

    private:
        SyncResult<value_type> val;
        IntResult(const IntResult::value_type& val);

    public:
        static AsyncResult<IntResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class StringResult
    {

    public:
        using value_type = std::string;

    private:
        SyncResult<value_type> val;
        StringResult(const StringResult::value_type& val);

    public:
        static AsyncResult<StringResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class BoolResult
    {
    public:
        using value_type = bool;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<BoolResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class VectorResult
    {
    public:
        using value_type = std::vector<Result>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<VectorResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class DoubleResult
    {
    public:
        using value_type = double;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<DoubleResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class DoubleVectorResult
    {
    public:
        using value_type = std::vector<double>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<DoubleVectorResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class IntVectorResult
    {
    public:
        using value_type = std::vector<std::int64_t>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<IntVectorResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class StringVectorResult
    {
    public:
        using value_type = std::vector<std::string>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<StringVectorResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class BoolVectorResult
    {
    public:
        using value_type = std::vector<bool>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<BoolVectorResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class MapResult
    {
    public:
        using value_type = std::map<std::string, Result>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<MapResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class MapStringStringResult
    {
    public:
        using value_type = std::map<std::string, std::string>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<MapStringStringResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

    class MapStringIntResult
    {
    public:
        using value_type = std::map<std::string, std::int64_t>;

    private:
        SyncResult<value_type> val;

    public:
        static AsyncResult<MapStringIntResult> create_from_connection(network::Connection &conn);

        const SyncResult<value_type> &result() const noexcept;

        value_type result(value_type default_value) const noexcept;
    };

} // namespace reddish::resp

#endif