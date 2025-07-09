cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

# set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 26)

project(rsl_util LANGUAGES CXX)

add_library(rsl_util INTERFACE)
target_compile_options(rsl_util INTERFACE 
  "-stdlib=libc++"
  "-freflection"
  "-fannotation-attributes"
  "-fparameter-reflection"
  "-fexpansion-statements"
  "-Wno-c++26-extensions"
)
target_link_options(rsl_util INTERFACE "-fexperimental-library" "-stdlib=libc++" "-lc++abi")

target_include_directories(rsl_util INTERFACE 
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/ DESTINATION include)

option(BUILD_TESTING "Enable tests" ON)
option(ENABLE_COVERAGE "Enable coverage instrumentation" OFF)
option(BUILD_EXAMPLES "Enable examples" ON)

if (BUILD_TESTING)
  message(STATUS "Building unit tests")

  enable_testing()
  add_executable(rsl_util_test "")
  add_subdirectory(test)

  find_package(GTest REQUIRED)
  target_link_libraries(rsl_util_test PRIVATE rsl_util)
  target_link_libraries(rsl_util_test PRIVATE GTest::gtest GTest::gmock)

  include(GoogleTest)
  gtest_discover_tests(rsl_util_test)


  if(ENABLE_COVERAGE)
    message(STATUS "Instrumenting for coverage")  

    target_compile_options(rsl_util_test PRIVATE -g -O0 --coverage)
    target_link_libraries(rsl_util_test PRIVATE --coverage)
  endif()
endif()

if (BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

