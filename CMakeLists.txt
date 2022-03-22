cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

project(
    CMakeNanobenchExample
    VERSION 1.0
    LANGUAGES CXX)

include(CTest)

add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE thirdparty/doctest)


add_executable(MyExample src/main.cpp)
target_link_libraries(MyExample PRIVATE doctest)

add_executable(tests src/test.cpp)
target_link_libraries(tests PUBLIC doctest)
enable_testing()
add_test(NAME Tests COMMAND tests)