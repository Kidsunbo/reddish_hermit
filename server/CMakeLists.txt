find_package(Boost REQUIRED)

add_executable(reddish_hermit_server)
add_subdirectory(src)
target_include_directories(reddish_hermit_server PUBLIC include)
target_link_libraries(reddish_hermit_server PUBLIC boost::boost reddish_hermit_common)


if(ENABLE_TEST)
    include(CTest)
    add_subdirectory(test)
    target_compile_options(reddish_hermit_server PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
    target_link_options(reddish_hermit_server PUBLIC -fsanitize=address --coverage)
endif()


install(TARGETS reddish_hermit_server)
