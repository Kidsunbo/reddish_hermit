cmake_minimum_required(VERSION 3.15)
project(reddish_hermit CXX)


set(CMAKE_CXX_STANDARD 20)

find_package(kie_toolbox REQUIRED)

add_library(reddish_hermit)
add_subdirectory(src)
target_include_directories(reddish_hermit PUBLIC include)
target_link_libraries(reddish_hermit PUBLIC kie_toolbox::kie_toolbox)


option(ENABLE_TEST "if enable test for reddish_hermit" ON)
if(ENABLE_TEST)
    include(CTest)
    add_subdirectory(test)
    target_compile_options(reddish_hermit PUBLIC -fsanitize=address -g --coverage -Wall -Wextra -Werror)
    target_link_options(reddish_hermit PUBLIC -fsanitize=address --coverage)
endif()


install(TARGETS reddish_hermit)
install(DIRECTORY include/ DESTINATION include)
install(FILES LICENSE DESTINATION license)
