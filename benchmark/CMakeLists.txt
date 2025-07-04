project(rsl)

function(DEFINE_EXAMPLE TARGET)
  add_executable(util_${TARGET} "${TARGET}.cpp")
  target_link_libraries(util_${TARGET} PRIVATE rsl)
  target_compile_options(util_${TARGET} PRIVATE "-O3")
endfunction()

# DEFINE_EXAMPLE(variant)
# DEFINE_EXAMPLE(trie)