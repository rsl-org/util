function(DEFINE_EXAMPLE TARGET)
  add_executable(example_${TARGET} "${TARGET}.cpp")
  target_link_libraries(example_${TARGET} PRIVATE rsl_util)
endfunction()

DEFINE_EXAMPLE(assert)

add_executable(example_review 
  review.cpp 
  review_tu2.cpp
)
target_link_libraries(example_review PRIVATE rsl_util)