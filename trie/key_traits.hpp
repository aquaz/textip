#ifndef TEXTIP_TRIE_KEY_TRAITS_H
#define TEXTIP_TRIE_KEY_TRAITS_H

#include <limits>
#include <type_traits>

namespace textip {
namespace trie_impl_ {

template <typename T>
struct key_traits {
  typedef typename T::value_type char_type;
  typedef std::make_unsigned_t<char_type> char_mapped_type;
  static constexpr char_mapped_type map_char(char_type c) { return c; }
  static constexpr std::size_t mapped_range =
    std::numeric_limits<char_mapped_type>::max() - std::numeric_limits<char_mapped_type>::min() + 1;
};
}
}

#endif
