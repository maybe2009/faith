add_executable(example test.cc)
target_link_libraries(example gtest_main)

enable_testing()
add_test(NAME example_test COMMAND example)
