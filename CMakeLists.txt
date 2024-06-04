cmake_minimum_required(VERSION 3.15)
project(reddish_hermit CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_TEST "if enable test for reddish_hermit" ON)
option(ENABLE_CLIENT "if compile and build the client" ON)
option(ENABLE_SERVER "if compile and bulild the server" ON)

add_subdirectory(common)

if(ENABLE_CLIENT)
    add_subdirectory(client)
endif()

if(ENABLE_SERVER)
    add_subdirectory(server)
endif()


install(FILES LICENSE DESTINATION license)
