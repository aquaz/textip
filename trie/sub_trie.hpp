#ifndef TEXTIP_TRIE_SUB_TRIE_H
#define TEXTIP_TRIE_SUB_TRIE_H

#include <tuple>

#include "../utils/class_helpers.hpp"

namespace textip {

template <typename Trie, bool Const>
class node_interface {
public:
  typedef constify<Trie, Const> trie_t;
  typedef constify<typename Trie::node_t, Const> node_t;
  typedef typename Trie::key_traits::key_type key_type;
  node_interface(std::nullptr_t = nullptr) {}
  node_interface(trie_t& trie, node_t* node)
    : trie_(&trie), node_(node) {

  }
  node_interface<Trie, false> remove_const() const {
    return node_interface<Trie, false>(const_cast<Trie&>(*trie_), const_cast<typename Trie::node_t*>(node_));
  }
  node_interface create(node_t* node) const {
    return node_interface(*trie_, node);
  }

  node_interface find(key_type const& key) const {
    return find(std::begin(key), std::end(key));
  }

  template <typename CharIt>
  node_interface find(CharIt begin, CharIt end) const {
    node_t* n = node_;
    CharIt it = begin;
    while (it != end && n) {
      std::tie(it, n) = n->find_child(*trie_, it, end);
    }
    return node_interface(*trie_, n);
  }

  operator bool() const { return node_; }
  node_interface parent() const { return create(node_->parent(*trie_)); }
  node_interface first_child() const { return create(node_->first_child(*trie_)); }
  node_interface next_child() const { return create(node_->next_child(*trie_)); }
  auto& value() const { return node_->value; }
  node_t* ptr() const { return node_; }
private:
  trie_t* trie_ = nullptr;
  node_t* node_ = nullptr;
};

}

#endif /* !TEXTIP_TRIE_SUB_TRIE_H */
