cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 26)

project(PackageTest CXX)
find_package(rsl CONFIG REQUIRED)

add_executable(example src/example.cpp)
target_compile_options(example PRIVATE 
  "-stdlib=libc++"
  "-freflection"
  "-fannotation-attributes"
  "-fparameter-reflection"
  "-fexpansion-statements"
  "-Wno-c++26-extensions"
)


target_link_libraries(example rsl::rsl)
