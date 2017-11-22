cmake_minimum_required(VERSION 3.5)

set(SRC echo_server.cc)
add_executable(echo_server ${SRC})
target_link_libraries(echo_server faithreactor faithnet)