find_package(Boost REQUIRED)

add_library(reddish_hermit_client)
add_subdirectory(src)
target_include_directories(reddish_hermit_client PUBLIC include)
target_link_libraries(reddish_hermit_client PUBLIC boost::boost reddish_hermit_common)


if(ENABLE_TEST)
    add_subdirectory(test)
    target_compile_options(reddish_hermit_client PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
    target_link_options(reddish_hermit_client PUBLIC -fsanitize=address --coverage)
endif()


install(TARGETS reddish_hermit_client)
install(DIRECTORY include/ DESTINATION include)