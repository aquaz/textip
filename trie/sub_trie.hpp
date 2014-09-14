#ifndef TEXTIP_TRIE_SUB_TRIE_H
#define TEXTIP_TRIE_SUB_TRIE_H

#include <type_traits>

#include "trie_node_proxy.hpp"

#include "../utils/class_helpers.hpp"

namespace textip {

template <typename Trie, bool Const, typename Enable = void> class sub_trie
    : public trie_node_proxy<Trie, Const> {
};

namespace {
GENERATE_HAS_MEMBER(cher_begin);
}

template <typename Trie, bool Const>
class sub_trie<Trie, Const, std::enable_if_t<has_member_cher_begin<typename Trie::node_t>::value>> {
public:
  typedef trie_node_proxy<Trie, Const> node_t;
  typedef typename Trie::char_iterator_t char_iterator_t;
  sub_trie() {}
  sub_trie(node_t const& node)
    : node_(node), char_it_(node_.char_begin()) {
  }

  auto c() const { return *char_it_; }
  sub_trie first_child() const {
    return (node_.char_end() - char_it_ <= 1) ?
           sub_trie(node_.first_child())
           : sub_trie(node_, char_it_ + 1);
  }
  sub_trie next_child() const {
    return (char_it_ == node_.char_begin()) ?
           sub_trie(node_.next_child())
           : sub_trie();
  }
  sub_trie parent() const {
    if (char_it_ == node_.char_begin()) {
      node_t p = node_.parent();
      if (!p)
        return sub_trie();
      char_iterator_t nit = p.char_end();
      return sub_trie(p, nit == p.char_begin() ? nit : nit - 1);
    }
    return sub_trie(node_, char_it_ - 1);
  }
  /*template <typename CharIt>
  sub_trie find(CharIt begin, CharIt end) const {

  }*/
private:
  sub_trie(node_t const& node, char_iterator_t char_it)
    : node_(node), char_it_(char_it) {
  }
  node_t node_;
  char_iterator_t char_it_ {};
};
}

#endif /* !TEXTIP_TRIE_SUB_TRIE_H */
