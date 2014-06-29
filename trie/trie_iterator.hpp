#ifndef TEXTIP_TRIE_ITERATOR_H
#define TEXTIP_TRIE_ITERATOR_H

#include <iterator>

#include "sub_trie.hpp"

namespace textip {
namespace trie_impl_ {

template <typename Trie, bool Const>
class trie_iterator : public std::iterator<std::forward_iterator_tag, constify<typename Trie::node_t::value_type, Const>> {
public:
  typedef node_interface<Trie, Const> node_t;
  friend class trie_iterator<Trie, true>;
  trie_iterator(trie_iterator<Trie, false> const& other) : trie_iterator(other.node_) {}
  trie_iterator() {}
  trie_iterator(node_t const& node) : node_(next_valid_node_(node)) {

  }

  trie_iterator<Trie, false> remove_const() const {
    return trie_iterator<Trie, false>(node_.remove_const());
  }

  typename trie_iterator::reference operator* () const {
    return *node_.value();
  }

  typename trie_iterator::pointer operator->() const {
    return &**this;
  }

  bool operator!= (trie_iterator const& other) const {
    return node_ != other.node_;
  }

  bool operator== (trie_iterator const& other) const {
    return node_ == other.node_;
  }

  trie_iterator& operator++ () {
    node_ = next_valid_node_(next_(node_));
    return *this;
  }
  trie_iterator operator++ (int) {
    auto tmp = *this;
    ++*this;
    return tmp;
  }
  node_t node() { return node_; }
private:
  // Find next node with a value (or itself if already valid)
  node_t next_valid_node_(node_t node) {
    while (node && !node.value()) {
      node = next_(node);
    }
    return node;
  }
  // Immediate next node or null
  node_t next_(node_t node) {
    node_t first_child = node.first_child();
    if (first_child) {
      return first_child;
    }
    while (node) {
      node_t adjacent = node.next_child();
      if (adjacent) {
        return adjacent;
      }
      node = node.parent();
    }
    return nullptr;
  }
  node_t node_;
};
}
}

#endif
