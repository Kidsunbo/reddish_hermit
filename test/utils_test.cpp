#include <gtest/gtest.h>
#include <reddish/utils/str.h>

using namespace reddish::utils;



TEST(StrTest, StrFunction){
    ASSERT_TRUE(case_insensitive_equal('A', 'a'));
    ASSERT_TRUE(case_insensitive_equal('b', 'B'));
    ASSERT_TRUE(case_insensitive_equal('a', 'a'));
    ASSERT_TRUE(case_insensitive_equal('B', 'B'));
    ASSERT_FALSE(case_insensitive_equal('a', '-'));
    ASSERT_FALSE(case_insensitive_equal('-', 'a'));
    ASSERT_TRUE(case_insensitive_equal(' ', ' '));
    ASSERT_TRUE(case_insensitive_equal('-', '-'));

    ASSERT_TRUE(case_insensitive_equal("", ""));
    ASSERT_TRUE(case_insensitive_equal("-", "-"));
    ASSERT_TRUE(case_insensitive_equal("Hello", "heLLo"));
    ASSERT_TRUE(case_insensitive_equal("Hello1", "Hello1"));
    ASSERT_FALSE(case_insensitive_equal("hello-","hello+"));

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}