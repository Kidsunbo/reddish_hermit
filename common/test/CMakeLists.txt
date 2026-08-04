find_package(GTest REQUIRED)

set(TEST_COMPILE_OPTIONS -fsanitize=address -g --coverage -Wall -Wextra -Werror -Wno-error=maybe-uninitialized)

function(reddish_add_test name)
    add_executable(${name} ${ARGN} gtest_main.cpp)
    target_link_libraries(${name} PUBLIC reddish_hermit_common GTest::gtest)
    target_compile_options(${name} PUBLIC ${TEST_COMPILE_OPTIONS})
    target_link_options(${name} PUBLIC -fsanitize=address --coverage)
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(${name} ${name})
endfunction()

reddish_add_test(connection_test connection/connection_test.cpp)
reddish_add_test(reader_test protocol/reader_test.cpp)
reddish_add_test(writer_test protocol/writer_test.cpp)
reddish_add_test(roundtrip_test protocol/roundtrip_test.cpp)
reddish_add_test(loopback_test protocol/loopback_test.cpp)
