target_sources(rsl-util-test PRIVATE 
    structured_bindings.cpp
    empty.cpp
    assign.cpp
    tie.cpp
    make_tuple.cpp
    cntr.cpp
)

add_subdirectory(tuple.get)
add_subdirectory(extensions)
