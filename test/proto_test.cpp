#include <gtest/gtest.h>
#include <reddish/proto/proto.h>

using namespace reddish::proto;



TEST(ProtoTest, ProtoFunction){
    auto v = to_request_str(Command::Get, "hello");
    ASSERT_EQ(v, "*2\r\n$3\r\nGET\r\n$5\r\nhello\r\n");

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}