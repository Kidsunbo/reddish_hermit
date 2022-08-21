find_package(GTest REQUIRED)

add_executable(proto_test proto_test.cpp)
target_link_libraries(proto_test PUBLIC reddish_hermit gtest::gtest)
target_compile_options(proto_test PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
target_link_options(proto_test PUBLIC -fsanitize=address --coverage)
add_test(proto_test proto_test)