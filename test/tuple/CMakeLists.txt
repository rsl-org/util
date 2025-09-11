target_sources(rsl-util-test PRIVATE 
    structured_bindings.cpp
    empty.cpp
)

add_subdirectory(tuple.get)
add_subdirectory(extensions)