#ifndef TEXTIP_UTILS_CLASS_HELPERS_H
#define TEXTIP_UTILS_CLASS_HELPERS_H

#include "mpl.hpp"

#include <utility>

#define NON_CONST_GETTER(method) \
  template <typename... U> \
  auto method(U&&... u) { \
    auto const* t = this;\
    return const_cast<remove_const_ref_t<decltype(t->method(std::forward<U>(u)...))>>( \
      t->method(std::forward<U>(u)...));\
  }

#define GENERATE_HAS_MEMBER(X) \
template <typename T> \
struct has_member_##X{ \
template <typename V> static constexpr bool value_(decltype(&V::X)) { return true; } \
template <typename V> static constexpr bool value_(...) { return false; } \
static constexpr bool value = value_<T>(nullptr); \
}

#endif /* !TEXTIP_UTILS_CLASS_HELPERS_H */
