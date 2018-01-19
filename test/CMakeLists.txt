cmake_minimum_required(VERSION 3.5)

set(SRC echo_server.cc)
add_executable(echo_server ${SRC})
target_link_libraries(echo_server faithreactor faithnet)

add_executable(rpccli rpccli.cc)
target_link_libraries(rpccli faithnet faithrpc)

add_executable(rpcsrv rpcsrv.cc)
target_link_libraries(rpcsrv faithnet faithrpc)
#add_executable(test_im_decoder test_im_decoder.cc)

add_executable(test_zzp test_zzp.cc)