cmake_minimum_required(VERSION 3.8)
project(stream)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
else()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
endif()
conan_basic_setup(TARGETS)

enable_testing()

add_library(stream INTERFACE)
target_sources(stream INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/action.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/callback.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/demultiplex.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/filter.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/transform.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/concepts/executor.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/concepts/pipe.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/concepts/stream.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/libstream/detail/context.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/liboutput_view/filter.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/liboutput_view/take_while.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/liboutput_view/transform.hpp)
target_include_directories(stream INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(stream INTERFACE cxx_std_17)
target_compile_options(stream INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-fconcepts>
    $<$<CXX_COMPILER_ID:Clang>:-Xclang -fconcepts-ts>)
target_link_libraries(stream INTERFACE CONAN_PKG::cmcstl2 CONAN_PKG::delegate)

add_subdirectory(tests)
