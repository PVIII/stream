cmake_minimum_required(VERSION 3.8)

add_executable(stream_test
               action_test.cpp
               transform_test.cpp)
target_link_libraries(stream_test PRIVATE stream CONAN_PKG::Catch2)
target_compile_options(stream_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)

add_test(NAME test COMMAND stream_test)
