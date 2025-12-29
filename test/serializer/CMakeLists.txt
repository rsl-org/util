target_sources(rsl-util-test PRIVATE 
  canonical_name.cpp
  to_owning.cpp
)

add_subdirectory(repr)
add_subdirectory(to_string)