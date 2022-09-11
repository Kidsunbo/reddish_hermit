#include <gtest/gtest.h>
#include <reddish/resp/protocol.h>

using namespace reddish::resp;



TEST(ProtocolTest, ProtocolFunction){
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


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}