#include <gtest/gtest.h>
#include <reddish/resp/protocol.h>

using namespace reddish::resp;

TEST(ProtocolTest, RequestClass)
{
  auto v = Request(Command::Get, "hello");
  ASSERT_EQ(v.to_string(), "*2\r\n$3\r\nGET\r\n$5\r\nhello\r\n");
  v = Request(Command::Set, "key", 10);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$2\r\n10\r\n");
  v = Request(Command::Set, "key", 100'000'000'000'000'000);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$18\r\n100000000000000000\r\n");
  v = Request(Command::Set, "key", 10U);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$2\r\n10\r\n");
  v = Request(Command::Set, "key", 100'000'000'000'000'000U);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$18\r\n100000000000000000\r\n");
  v = Request(Command::Set, "key", -10);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$3\r\n-10\r\n");
  v = Request(Command::Set, "key", -100'000'000'000'000'000);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$19\r\n-100000000000000000\r\n");
  v = Request(Command::Set, "key", 20.1);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$4\r\n20.1\r\n");
  v = Request(Command::Set, "key", 3.141596f);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$6\r\n3.1416\r\n");
  v = Request(Command::Set, "key", 3.14159265359L);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$7\r\n3.14159\r\n");
  v = Request(Command::Set, "key", static_cast<unsigned short>(10));
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$2\r\n10\r\n");
  v = Request(Command::Set, "key", static_cast<short>(-10));
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$3\r\n-10\r\n");
  v = Request(Command::Set, "key", true);
  ASSERT_EQ(v.to_string(), "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$4\r\ntrue\r\n");
}

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
    auto v = empty_result.as_int64_vector({1,2,3,4});
    ASSERT_EQ(v.size(), 4);
  }
  {
    auto v = empty_result.as_uint64_vector({3,4,5,6});
    ASSERT_EQ(v.size(), 4);
  }
  {
    auto v = empty_result.as_float_vector({1.1,2.2,3.3});
    ASSERT_EQ(v.size(), 3);
    ASSERT_FLOAT_EQ(v[0], 1.1);
  }
  {
    auto v = empty_result.as_double_vector({1.2,3.4});
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


TEST(ProtocolTest, FromStringCopyClass){
  std::string array_str = "*5\r\n$3\r\nabc\r\n:1\r\n- Err This is a error test\r\n+123123\r\n*3\r\n:1\r\n*2\r\n$1\r\nh\r\n:10\r\n+ping\r\n";
  auto result = Result::from_string(array_str);
  ASSERT_FALSE(result.has_error());
  auto v = result.value().as_vector();
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value().size(), 5);
  ASSERT_EQ(v.value().at(0).as_string(""), "abc");
  ASSERT_FALSE(v.value().at(1).as_int64().has_error());
  ASSERT_EQ(v.value().at(1).as_int64().value(), 1);
  ASSERT_EQ(v.value().at(2).as_string(""), " Err This is a error test");
  ASSERT_EQ(v.value().at(3).as_int32(0), 123123);
  v = v.value().at(4).as_vector();
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value().size(), 3);
  ASSERT_EQ(v.value().at(2).as_string().value(), "ping");




}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}