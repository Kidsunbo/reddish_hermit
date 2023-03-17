find_package(GTest REQUIRED)

add_executable(request_test request_test.cpp)
target_link_libraries(request_test PUBLIC reddish_hermit gtest::gtest)
target_compile_options(request_test PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
target_link_options(request_test PUBLIC -fsanitize=address --coverage)
add_test(request_test request_test)

add_executable(response_test response_test.cpp)
target_link_libraries(response_test PUBLIC reddish_hermit gtest::gtest)
target_compile_options(response_test PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
target_link_options(response_test PUBLIC -fsanitize=address --coverage)
add_test(response_test response_test)


add_executable(connection_test connection_test.cpp)
target_link_libraries(connection_test PUBLIC reddish_hermit gtest::gtest)
target_compile_options(connection_test PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
target_link_options(connection_test PUBLIC -fsanitize=address --coverage)
add_test(connection_test connection_test)


add_executable(utils_test utils_test.cpp)
target_link_libraries(utils_test PUBLIC reddish_hermit gtest::gtest)
target_compile_options(utils_test PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
target_link_options(utils_test PUBLIC -fsanitize=address --coverage)
add_test(utils_test utils_test)