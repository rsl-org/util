target_sources(rsl_test PRIVATE 
    structured_bindings.cpp
    empty.cpp
)

add_subdirectory(tuple.get)
add_subdirectory(extensions)