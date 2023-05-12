#include <gtest/gtest.h>
#include <reddish/utils/string.h>

using namespace reddish::utils;



TEST(StrTest, CaseInsensitiveEqualFunction){
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

TEST(StrTest, RetrieveLengthFunction){

  std::string simple_string1 = "+ping\r\n";
  auto it = simple_string1.cbegin() + 1;
  auto v = retrieve_length(it, simple_string1.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, 'p');

  std::string simple_string2 = ":123\r\n";
  it = simple_string2.cbegin() + 1;
  v = retrieve_length(it, simple_string2.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 123);
  ASSERT_EQ(it, simple_string2.cend());

  std::string error_string1 = "-Err hello world\r\n";
  it = error_string1.cbegin() + 1;
  v = retrieve_length(it, error_string1.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, 'E');

  std::string bulk_string1 = "$-1\r\n";
  it = bulk_string1.cbegin() + 1;
  v = retrieve_length(it, bulk_string1.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), -1);
  ASSERT_EQ(it, bulk_string1.cend());

  std::string bulk_string2 = "$0\r\n\r\n";
  it = bulk_string2.cbegin() + 1;
  v = retrieve_length(it, bulk_string2.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 0);
  ASSERT_EQ(*it,'\r');

  std::string bulk_string3 = "$1\r\nh\r\n";
  it = bulk_string3.cbegin() + 1;
  v = retrieve_length(it, bulk_string3.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 1);
  ASSERT_EQ(*it, 'h');


  std::string bulk_string4 = "$11\r\nhello world\r\n";
  it = bulk_string4.cbegin() + 1;
  v = retrieve_length(it, bulk_string4.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 11);
  ASSERT_EQ(*it, 'h');

  std::string bulk_string5 = "$14\r\nit is\r\na\r\ntest\r\n";
  it = bulk_string5.cbegin() + 1;
  v = retrieve_length(it, bulk_string5.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 14);
  ASSERT_EQ(*it, 'i');

  std::string abnormal_string1 = "";
  it = abnormal_string1.cbegin();
  v = retrieve_length(it, abnormal_string1.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(it, abnormal_string1.cend());

  std::string abnormal_string2 = "$";
  it = abnormal_string2.cbegin();
  v = retrieve_length(it, abnormal_string2.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, '$');

  std::string abnormal_string3 = "$-10\r\n";
  it = abnormal_string3.cbegin();
  v = retrieve_length(it, abnormal_string3.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, '$');

  std::string abnormal_string3_1 = "$-10\r\n";
  it = abnormal_string3_1.cbegin() + 1;
  v = retrieve_length(it, abnormal_string3_1.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), -10);
  ASSERT_EQ(it, abnormal_string3_1.cend());

  std::string abnormal_string4 = "1\r\n";
  it = abnormal_string4.cbegin();
  v = retrieve_length(it, abnormal_string4.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 1);
  ASSERT_EQ(it, abnormal_string4.cend());

  std::string abnormal_string4_1 = "1\r\n";
  it = abnormal_string4_1.cbegin() + 1;
  v = retrieve_length(it, abnormal_string4_1.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 0);
  ASSERT_EQ(it, abnormal_string4_1.cend());

  std::string abnormal_string5 = "$\r\n\r\n";
  it = abnormal_string5.cbegin();
  v = retrieve_length(it, abnormal_string5.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, '$');


  std::string abnormal_string6 = "$$$$$$$$\r\n";
  it = abnormal_string6.cbegin();
  v = retrieve_length(it, abnormal_string6.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, '$');

  std::string abnormal_string7 = "123";
  it = abnormal_string7.cbegin();
  v = retrieve_length(it, abnormal_string7.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(it, abnormal_string7.cend());

  std::string abnormal_string8 = "123\r";
  it = abnormal_string8.cbegin();
  v = retrieve_length(it, abnormal_string8.cend());
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, '\r');
}

TEST(StrTest, RetrieveStringWithoutAdvanceFunction){
  std::string left_string1 = "hello\r\n";
  auto it = left_string1.cbegin();
  auto v = retrieve_string_without_advance(it, left_string1.cend(), 5);
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(*it, 'h');

  std::string left_string2 = "hello\r\n";
  it = left_string2.cbegin();
  v = retrieve_string_without_advance(it, left_string2.cend(), -5);
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, 'h');

  std::string left_string3 = "hello\r\n";
  it = left_string3.cbegin();
  v = retrieve_string_without_advance(it, left_string3.cend(), -1);
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), "nil");
  ASSERT_EQ(*it, 'h');

  std::string left_string4 = "h\r\n";
  it = left_string4.cbegin();
  v = retrieve_string_without_advance(it, left_string4.cend(), 2);
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), "h\r");
  ASSERT_EQ(*it, 'h');

  std::string left_string5 = "h\r\n";
  it = left_string5.cbegin();
  v = retrieve_string_without_advance(it, left_string5.cend(), 4);
  ASSERT_TRUE(v.has_error());
  ASSERT_EQ(*it, 'h');
}

TEST(StrTest, AdvanceIfSameFunction){
  std::string left_string1 = "hello\r\n";
  auto it = left_string1.cbegin();
  auto v = advance_if_same(it, left_string1.cend(), "hello");
  ASSERT_TRUE(v);
  ASSERT_EQ(*it, '\r');

  std::string left_string2 = "hello\r\n";
  it = left_string2.cbegin();
  v = advance_if_same(it, left_string2.cend(), "he");
  ASSERT_TRUE(v);
  ASSERT_EQ(*it, 'l');

  std::string left_string3 = "hello\r\n";
  it = left_string3.cbegin();
  v = advance_if_same(it, left_string3.cend(), "ello");
  ASSERT_FALSE(v);
  ASSERT_EQ(*it, 'h');

  std::string left_string4 = "hello\r\n";
  it = left_string4.cbegin();
  v = advance_if_same(it, left_string4.cend(), "hello\r\n\r");
  ASSERT_FALSE(v);
  ASSERT_EQ(*it, 'h');

  std::string left_string5 = "h\r\n";
  it = left_string5.cbegin();
  v = advance_if_same(it, left_string5.cend(), "\r\n");
  ASSERT_FALSE(v);
  ASSERT_EQ(*it, 'h');
}

TEST(StrTest, AdvanceToNextLineFunction){
  std::string left_string1 = "hello\r\nworld";
  auto it = left_string1.cbegin();
  auto v = advance_to_next_line(it, left_string1.cend());
  ASSERT_EQ(v, 7);
  ASSERT_EQ(*it, 'w');

  std::string left_string2 = "hello\r\n";
  it = left_string2.cbegin();
  v = advance_to_next_line(it, left_string2.cend());
  ASSERT_EQ(v, 7);
  ASSERT_EQ(it, left_string2.cend());

  std::string left_string3 = "";
  it = left_string3.cbegin();
  v = advance_to_next_line(it, left_string3.cend());
  ASSERT_EQ(v, 0);
  ASSERT_EQ(it, left_string3.cend());

  std::string left_string4 = "h";
  it = left_string4.cbegin();
  v = advance_to_next_line(it, left_string4.cend());
  ASSERT_EQ(v, 0);
  ASSERT_EQ(*it, 'h');

  std::string left_string6 = "h\r\n";
  it = left_string6.cbegin();
  v = advance_to_next_line(it, left_string6.cend());
  ASSERT_EQ(v, 3);
  ASSERT_EQ(it, left_string6.cend());

  std::string left_string7 = "\r";
  it = left_string7.cbegin();
  v = advance_to_next_line(it, left_string7.cend());
  ASSERT_EQ(v, 0);
  ASSERT_EQ(*it, '\r');

  std::string left_string8 = "\r\n";
  it = left_string8.cbegin();
  v = advance_to_next_line(it, left_string8.cend());
  ASSERT_EQ(v, 2);
  ASSERT_EQ(it, left_string8.cend());
}


TEST(StrTest, NextItemLengthFunction){
  std::string empty_string = "";
  auto v = next_item_length(empty_string.cbegin(), empty_string.cend());
  ASSERT_TRUE(v.has_error());

  std::string wrong_string = "hello";
  v = next_item_length(wrong_string.cbegin(), wrong_string.cend());
  ASSERT_TRUE(v.has_error());

  std::string simple_string = "+hello";
  v = next_item_length(simple_string.cbegin(), simple_string.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 5);

  std::string simple_string1 = "+hello\r";
  v = next_item_length(simple_string1.cbegin(), simple_string1.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 6);

  std::string simple_string2 = "+hello\r\n";
  v = next_item_length(simple_string2.cbegin(), simple_string2.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 8);

  std::string simple_string3 = "+hello\r\n\r\n";
  v = next_item_length(simple_string3.cbegin(), simple_string3.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 8);

  std::string error_string = "-hello";
  v = next_item_length(error_string.cbegin(), error_string.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 5);

  std::string bulk_string = "$12\r\nhello\r\nworld\r\n";
  v = next_item_length(bulk_string.cbegin(), bulk_string.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 19);

  std::string error_bulk_string = "$12\r\nhello\r\nwor";
  v = next_item_length(error_bulk_string.cbegin(), error_bulk_string.cend());
  ASSERT_TRUE(v.has_error());

  std::string array_string = "*2\r\n$-1\r\n:456\r\n";
  v = next_item_length(array_string.cbegin(), array_string.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 15);

  std::string array_string1 = "*3\r\n$-1\r\n:123\r\n-Err hello world and this is a test \r\n";
  v = next_item_length(array_string1.cbegin(), array_string1.cend());
  ASSERT_FALSE(v.has_error());
  ASSERT_EQ(v.value(), 53);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}