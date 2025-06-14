target_sources(rsl_test PRIVATE main.cpp)
target_include_directories(rsl_test PRIVATE .)

add_subdirectory(variant)
add_subdirectory(tagged_variant)
add_subdirectory(tuple)