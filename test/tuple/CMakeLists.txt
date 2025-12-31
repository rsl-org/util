target_sources(rsl-util-test PRIVATE
    cntr.cpp
    structured_bindings.cpp
    empty.cpp
    assign.cpp
    tie.cpp
    make_tuple.cpp
)

add_subdirectory(tuple.get)
add_subdirectory(extensions)
