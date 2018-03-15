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

add_executable(test_rio test_rio.cc)

add_executable(test-client-api client-api.cc)
target_link_libraries(test-client-api faithnet)

add_executable(test-server-api server-api.cc)
target_link_libraries(test-server-api faithnet)

add_executable(test-connect nonblocking-connection.cc)
target_link_libraries(test-connect faithnet)

add_executable(test-epoll-server epoll-server.cc)
target_link_libraries(test-epoll-server faithnet)

add_executable(test-epoll-client epoll-client.cc)
target_link_libraries(test-epoll-client faithnet)