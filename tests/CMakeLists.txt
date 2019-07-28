cmake_minimum_required(VERSION 3.8)

add_executable(action_test
               ../libstream/action.test.cpp)
target_link_libraries(action_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::FakeIt)
target_compile_options(action_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(action_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(action_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)

add_test(NAME action_test COMMAND action_test)

add_executable(transform_test
               ../libstream/transform.test.cpp)
target_link_libraries(transform_test PRIVATE stream CONAN_PKG::prebuilt-catch2)
target_compile_options(transform_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(transform_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(transform_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)

add_test(NAME transform_test COMMAND transform_test)

add_executable(view_test
               ../liboutput_view/transform.test.cpp)
target_link_libraries(view_test PRIVATE stream CONAN_PKG::prebuilt-catch2)
target_compile_options(view_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)

add_test(NAME view_test COMMAND view_test)
