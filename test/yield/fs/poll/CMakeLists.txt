file(GLOB SRC *.cpp *.hpp)
source_group("Source Files" FILES ${SRC})
add_executable(yield.fs.poll_test ${SRC})
add_test(yield.fs.poll_test yield.fs.poll_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.fs.poll_test.xml)
target_link_libraries(yield.fs.poll_test yield.fs.poll gtest)
