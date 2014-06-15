#ifndef TEXTIP_UTILS_MPL_H
#define TEXTIP_UTILS_MPL_H

#include <type_traits>

namespace textip {

template <typename T, bool Const>
using constify = typename std::conditional<Const, const T, T>::type;

template <typename T>
struct remove_const_ref {
  typedef T type;
};

template <typename T>
struct remove_const_ref<T const*> {
  typedef T* type;
};

template <typename T>
struct remove_const_ref<T const&> {
  typedef T& type;
};

template <typename T>
using remove_const_ref_t = typename remove_const_ref<T>::type;

}

#endif /* !TEXTIP_UTILS_MPL_H */
