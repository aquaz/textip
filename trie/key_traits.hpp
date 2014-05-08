#ifndef TEXTIP_TRIE_KEY_TRAITS_H
#define TEXTIP_TRIE_KEY_TRAITS_H

namespace textip {
namespace trie_impl_ {

template <typename T>
struct key_traits {
  typedef typename T::value_type char_type;
  typedef typename T::const_iterator iterator;
};
}
}

#endif
