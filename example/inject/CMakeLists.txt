add_executable(injector injection_impl.cpp)
target_link_libraries(injector PRIVATE rsl-inject)
