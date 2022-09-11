find_package(GTest REQUIRED)

add_executable(resp_test resp_test.cpp)
target_link_libraries(resp_test PUBLIC reddish_hermit gtest::gtest)
target_compile_options(resp_test PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
target_link_options(resp_test PUBLIC -fsanitize=address --coverage)
add_test(resp_test resp_test)


add_executable(connection_test connection_test.cpp)
target_link_libraries(connection_test PUBLIC reddish_hermit gtest::gtest)
target_compile_options(connection_test PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
target_link_options(connection_test PUBLIC -fsanitize=address --coverage)
add_test(connection_test connection_test)
