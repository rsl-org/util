target_sources(rsl_util_test PRIVATE main.cpp)
target_include_directories(rsl_util_test PRIVATE .)

add_subdirectory(variant)
add_subdirectory(tagged_variant)
add_subdirectory(tuple)
add_subdirectory(expect)