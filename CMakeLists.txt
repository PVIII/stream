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
            ${CMAKE_CURRENT_SOURCE_DIR}/include/stream/action.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/stream/transform.h)
target_include_directories(stream INTERFACE include/)
target_compile_features(stream INTERFACE cxx_std_17)
target_compile_options(stream INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-fconcepts>
    $<$<CXX_COMPILER_ID:Clang>:-Xclang -fconcepts-ts>)
target_link_libraries(stream INTERFACE CONAN_PKG::cmcstl2 CONAN_PKG::delegate)

add_subdirectory(test)
