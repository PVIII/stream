cmake_minimum_required(VERSION 3.8)

add_executable(stream_test
               action_test.cpp
               transform_test.cpp)
target_link_libraries(stream_test PRIVATE stream CONAN_PKG::Catch2)
target_compile_options(stream_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(stream_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
# Replace this with target_link_options starting with CMake 13.
set_target_properties(stream_test PROPERTIES LINK_FLAGS "-fsanitize=address -fsanitize=leak -fsanitize=undefined")

add_test(NAME test COMMAND stream_test)
