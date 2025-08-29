cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

# set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(rsl_util VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

add_library(rsl_util INTERFACE)
add_library(rsl::util ALIAS rsl_util)
target_compile_options(rsl_util INTERFACE
  "-stdlib=libc++"
  "-freflection"
  "-fannotation-attributes"
  "-fparameter-reflection"
  "-fexpansion-statements"
  "-Wno-c++26-extensions"
)
target_compile_features(rsl_util INTERFACE cxx_std_26)
target_link_options(rsl_util INTERFACE "-fexperimental-library" "-stdlib=libc++")
target_link_libraries(rsl_util INTERFACE "c++abi")
target_include_directories(rsl_util INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

option(BUILD_TESTING "Enable tests" ON)
option(ENABLE_COVERAGE "Enable coverage instrumentation" OFF)
option(BUILD_EXAMPLES "Enable examples" ON)
option(RSL_UTIL_INSTALL "Generate install target for rsl_util" ON)

if (RSL_UTIL_INSTALL)
  set(CMAKE_INSTALLDIR_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/rsl-util)

  set_target_properties(rsl_util PROPERTIES EXPORT_NAME util)
  install(TARGETS rsl_util EXPORT rsl-util-targets)
  install(EXPORT rsl-util-targets
    NAMESPACE rsl::
    DESTINATION ${CMAKE_INSTALLDIR_CMAKEDIR}
  )

  include(CMakePackageConfigHelpers)
  configure_package_config_file(cmake/rsl-util-config.cmake.in
    ${PROJECT_BINARY_DIR}/rsl-util-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALLDIR_CMAKEDIR}/rsl-util-config.cmake
  )
  write_basic_package_version_file(${PROJECT_BINARY_DIR}/rsl-util-config-version.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
  )
  install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
  install(FILES
    ${PROJECT_BINARY_DIR}/rsl-util-config-version.cmake
    ${PROJECT_BINARY_DIR}/rsl-util-config.cmake
    DESTINATION ${CMAKE_INSTALLDIR_CMAKEDIR}
  )
endif()

if (BUILD_TESTING)
  message(STATUS "Building unit tests")

  enable_testing()
  add_executable(rsl_util_test "")
  add_subdirectory(test)

  find_package(GTest REQUIRED)
  target_link_libraries(rsl_util_test PRIVATE rsl_util)
  target_link_libraries(rsl_util_test PRIVATE GTest::gtest)

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

