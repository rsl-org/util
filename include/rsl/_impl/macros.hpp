#pragma once
#define RSL_IMPL_PROBE(...)        __VA_ARGS__, 1
#define RSL_IMPL_SECOND(a, b, ...) b
#define RSL_IMPL_IS_PROBE(...)     RSL_IMPL_SECOND(__VA_ARGS__, 0)
#define RSL_IMPL_CAT(a, b)         a##b
#define RSL_IMPL_IS_EMPTY(x)       RSL_IMPL_IS_PROBE(RSL_IMPL_CAT(RSL_IMPL_IS_EMPTY_HELPER_, x))
#define RSL_IMPL_IS_EMPTY_HELPER_  RSL_IMPL_PROBE(~)
