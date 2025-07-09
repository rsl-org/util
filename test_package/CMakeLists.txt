cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 26)

project(PackageTest CXX)
find_package(rsl-util REQUIRED)

add_executable(example src/example.cpp)
target_compile_options(example PRIVATE "-freflection-latest")

target_link_libraries(example rsl::util)
