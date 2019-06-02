cmake_minimum_required(VERSION 3.8)

add_executable(stream_test
               action_test.cpp
               transform_test.cpp)
target_link_libraries(stream_test PRIVATE stream CONAN_PKG::prebuilt-catch2)
target_compile_options(stream_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(stream_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(stream_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)

add_test(NAME stream_test COMMAND stream_test)

add_executable(view_test
               transform_view_test.cpp)
target_link_libraries(view_test PRIVATE stream CONAN_PKG::prebuilt-catch2)
target_compile_options(view_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)

add_test(NAME view_test COMMAND view_test)
