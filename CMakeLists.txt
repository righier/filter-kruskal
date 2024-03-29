cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

project(
    CMakeNanobenchExample
    VERSION 1.0
    LANGUAGES CXX)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

include(CTest)

add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE thirdparty/doctest)

add_library(nanobench INTERFACE)
target_include_directories(nanobench INTERFACE thirdparty/nanobench)

add_executable(MyExample src/main.cpp)
target_link_libraries(MyExample PRIVATE doctest nanobench)

add_executable(tests src/test.cpp)
target_link_libraries(tests PRIVATE doctest nanobench)
add_test(NAME Tests COMMAND tests)
enable_testing()