find_package(Boost REQUIRED)

add_library(reddish_hermit_common)
add_subdirectory(src)
target_include_directories(reddish_hermit_common PUBLIC include)
target_link_libraries(reddish_hermit_common PUBLIC boost::boost)


if(ENABLE_TEST)
    add_subdirectory(test)
    target_compile_options(reddish_hermit_common PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
    target_link_options(reddish_hermit_common PUBLIC -fsanitize=address --coverage)
endif()