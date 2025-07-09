target_sources(rsl_util_test PRIVATE 
    structured_bindings.cpp
    empty.cpp
)

add_subdirectory(tuple.get)
add_subdirectory(extensions)