project(rsl)

function(DEFINE_EXAMPLE TARGET)
  add_executable(example_${TARGET} "${TARGET}.cpp")
  target_link_libraries(example_${TARGET} PRIVATE rsl)
endfunction()

DEFINE_EXAMPLE(constexpr_assert)