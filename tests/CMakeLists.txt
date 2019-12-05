cmake_minimum_required(VERSION 3.8)

add_executable(action_test
               ../libstream/action.test.cpp)
target_link_libraries(action_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(action_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(action_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(action_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME action_test COMMAND action_test)

add_executable(demultiplex_test ../libstream/demultiplex.test.cpp)
target_link_libraries(demultiplex_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(demultiplex_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(demultiplex_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(demultiplex_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME demultiplex_test COMMAND demultiplex_test)

add_executable(filter_test ../libstream/filter.test.cpp)
target_link_libraries(filter_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(filter_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(filter_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(filter_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME filter_test COMMAND filter_test)

add_executable(pipe_test ../libstream/pipe.test.cpp)
target_link_libraries(pipe_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(pipe_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(pipe_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(pipe_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME pipe_test COMMAND pipe_test)

add_executable(take_until_test
               ../libstream/take_until.test.cpp)
target_link_libraries(take_until_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(take_until_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(take_until_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(take_until_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME take_until_test COMMAND take_until_test)

add_executable(transform_test
               ../libstream/transform.test.cpp)
target_link_libraries(transform_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(transform_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(transform_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(transform_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME transform_test COMMAND transform_test)

add_executable(interoperability_test interoperability.test.cpp)
target_link_libraries(interoperability_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(interoperability_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(interoperability_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(interoperability_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME interoperability_test COMMAND interoperability_test)

add_executable(filter_view_test
               ../liboutput_view/filter.test.cpp)
target_link_libraries(filter_view_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(filter_view_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(filter_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(filter_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME filter_view_test COMMAND filter_view_test)

add_executable(take_until_view_test
               ../liboutput_view/take_until.test.cpp)
target_link_libraries(take_until_view_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(take_until_view_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(take_until_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(take_until_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME take_until_view_test COMMAND take_until_view_test)

add_executable(take_while_view_test
               ../liboutput_view/take_while.test.cpp)
target_link_libraries(take_while_view_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(take_while_view_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(take_while_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(take_while_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME take_while_view_test COMMAND take_while_view_test)

add_executable(transform_view_test
               ../liboutput_view/transform.test.cpp)
target_link_libraries(transform_view_test PRIVATE stream CONAN_PKG::prebuilt-catch2 CONAN_PKG::trompeloeil)
target_compile_options(transform_view_test PRIVATE -pedantic-errors -Werror -Wall -Wextra)
target_compile_options(transform_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
target_link_options(transform_view_test PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
add_test(NAME transform_view_test COMMAND transform_view_test)
