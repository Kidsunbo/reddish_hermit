#include <gtest/gtest.h>
#include <reddish/resp/response.h>

using namespace reddish::resp;

TEST(ProtocolTest, EmptyResultClass)
{
  Result empty_result;
  {
    auto v = empty_result.as_string();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_int32();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_int64();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_uint32();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_uint64();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_boolean();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_float();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_double();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_vector();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_string_vector();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_int64_vector();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_uint64_vector();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_float_vector();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_double_vector();
    ASSERT_TRUE(v.has_error());
  }
  {
    auto v = empty_result.as_boolean_vector();
    ASSERT_TRUE(v.has_error());
  }

  {
    auto v = empty_result.as_string("hello");
    ASSERT_EQ(v, "hello");
  }
  {
    auto v = empty_result.as_int32(1);
    ASSERT_EQ(v, 1);
  }
  {
    auto v = empty_result.as_int64(-2);
    ASSERT_EQ(v, -2);
  }
  {
    auto v = empty_result.as_uint32(3);
    ASSERT_EQ(v, 3);
  }
  {
    auto v = empty_result.as_uint64(4);
    ASSERT_EQ(v, 4);
  }
  {
    auto v = empty_result.as_boolean(true);
    ASSERT_EQ(v, true);
  }
  {
    auto v = empty_result.as_float(1.1);
    ASSERT_FLOAT_EQ(v, 1.1);
  }
  {
    auto v = empty_result.as_double(1.234);
    ASSERT_DOUBLE_EQ(v, 1.234);
  }
  {
    auto v = empty_result.as_vector({Result{}});
    ASSERT_EQ(v.size(), 1);
  }
  {
    auto v = empty_result.as_string_vector({"hello world"});
    ASSERT_EQ(v.size(), 1);
    ASSERT_EQ(v[0], "hello world");
  }
  {
    auto v = empty_result.as_int64_vector({1, 2, 3, 4});
    ASSERT_EQ(v.size(), 4);
  }
  {
    auto v = empty_result.as_uint64_vector({3, 4, 5, 6});
    ASSERT_EQ(v.size(), 4);
  }
  {
    auto v = empty_result.as_float_vector({1.1, 2.2, 3.3});
    ASSERT_EQ(v.size(), 3);
    ASSERT_FLOAT_EQ(v[0], 1.1);
  }
  {
    auto v = empty_result.as_double_vector({1.2, 3.4});
    ASSERT_EQ(v.size(), 2);
    ASSERT_DOUBLE_EQ(v[0], 1.2);
  }
  {
    auto v = empty_result.as_boolean_vector({true, false});
    ASSERT_EQ(v.size(), 2);
    ASSERT_EQ(v[0], true);
    ASSERT_EQ(v[1], false);
  }
}

TEST(ProtocolTest, FromStringCopyClass)
{
  std::string array_str = "*5\r\n$3\r\nabc\r\n:1\r\n- Err This is a error test\r\n+123123\r\n*3\r\n$-1\r\n*2\r\n$1\r\nh\r\n:10\r\n+ping\r\n";
  auto result = Result::from_string(array_str);
  ASSERT_FALSE(result.has_error());
  {
    auto v = result.value().as_vector();
    ASSERT_FALSE(v.has_error());
    ASSERT_EQ(v.value().size(), 5);
    ASSERT_EQ(v.value().at(0).as_string(""), "abc");
    ASSERT_FALSE(v.value().at(1).as_int64().has_error());
    ASSERT_EQ(v.value().at(1).as_int64().value(), 1);
    ASSERT_EQ(v.value().at(2).as_string(""), " Err This is a error test");
    ASSERT_EQ(v.value().at(3).as_int32(0), 123123);
  }
  {
    auto v = result.value().as_vector().value().at(4).as_vector();
    ASSERT_FALSE(v.has_error());
    ASSERT_EQ(v.value().size(), 3);
    ASSERT_TRUE(v.value().at(0).is_null());
    ASSERT_FALSE(v.value().at(0).as_string().has_error());
    ASSERT_EQ(v.value().at(0).as_string().value(), "nil");
    ASSERT_TRUE(v.value().at(0).as_boolean().has_error());
    ASSERT_EQ(v.value().at(2).as_string().value(), "ping");
  }
  {
    auto v = result.value().as_string_vector();
    ASSERT_TRUE(v.has_error());
    std::string string_array = "*5\r\n:1\r\n+222\r\n-Error hello\r\n$5\r\nhello\r\n:hello\r\n";
    auto string_result = Result::from_string(string_array);
    ASSERT_FALSE(string_result.has_error());
    ASSERT_EQ(string_result.value().type(), Result::ResultType::Array);
    ASSERT_FALSE(string_result.value().as_vector().has_error());
    ASSERT_EQ(string_result.value().as_vector().value().at(4).type(), Result::ResultType::Integer);
    ASSERT_TRUE(string_result.value().as_vector().value().at(4).as_int64().has_error());
    ASSERT_FALSE(string_result.value().as_vector().value().at(4).as_string().has_error());
  }
  {
    std::string array_str = "*5\r";
    auto result = Result::from_string(array_str);
    ASSERT_TRUE(result.has_error());
    ASSERT_EQ(array_str, "*5\r"); // make sure the array_str is not moved.
  }
  {
    std::string array_str = "*5\r\n+true\r\n:1\r\n$1\r\nt\r\n+false\r\n:0\r\n";
    auto result = Result::from_string(array_str);
    ASSERT_FALSE(result.has_error());
    auto v = result.value().as_boolean_vector();
    ASSERT_FALSE(v.has_error());
    ASSERT_TRUE(v.value().at(0));
    ASSERT_TRUE(v.value().at(1));
    ASSERT_TRUE(v.value().at(2));
    ASSERT_FALSE(v.value().at(3));
    ASSERT_FALSE(v.value().at(4));
  }
}

TEST(ProtocolTest, FromStringMoveClass)
{
  {
    std::string array_str = "*5\r";
    auto result = Result::from_string(std::move(array_str));
    ASSERT_TRUE(result.has_error());
    ASSERT_EQ(array_str, "*5\r"); // make sure the array_str is not moved when parsing failed.
  }

  {
    std::string array_str = ":5\r\n";
    auto result = Result::from_string(std::move(array_str));
    ASSERT_FALSE(result.has_error());
    ASSERT_EQ(array_str, ""); // make sure the array_str is moved when parsing success.
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}