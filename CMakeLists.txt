cmake_minimum_required(VERSION 3.5)
project(epoll)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
set(CMAKE_CXX_STANDARD 11)

include_directories(src/epoll)
include_directories(src/net)
include_directories(src/utli)

add_subdirectory(src/epoll)
add_subdirectory(src/net)
add_subdirectory(test)

