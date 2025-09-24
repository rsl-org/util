target_sources(rsl-util-test PRIVATE 
  lifetime.cpp 
  special_members.cpp
  swap.cpp)

add_subdirectory(variant.assign)
add_subdirectory(variant.bad.access)
add_subdirectory(variant.get)
add_subdirectory(variant.helper)
add_subdirectory(variant.visit)
add_subdirectory(variant.ctor)
add_subdirectory(variant.hash)
