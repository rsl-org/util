project(rsl)

function(DEFINE_EXAMPLE TARGET)
  add_executable(${TARGET} "${TARGET}.cpp")
  target_link_libraries(${TARGET} PRIVATE rsl)
endfunction()

