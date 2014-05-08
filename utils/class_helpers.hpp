#ifndef TEXTIP_UTILS_CLASS_HELPERS_H
#define TEXTIP_UTILS_CLASS_HELPERS_H

#include "mpl.hpp"

#define NON_CONST_GETTER(method) \
  auto method() { \
    auto const* t = this;\
    return const_cast<typename remove_const_ref<decltype(t->method())>::type>(t->method());\
  }

#endif /* !TEXTIP_UTILS_CLASS_HELPERS_H */
