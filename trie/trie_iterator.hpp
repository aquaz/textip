#ifndef TEXTIP_TRIE_ITERATOR_H
#define TEXTIP_TRIE_ITERATOR_H

#include <iterator>
#include <iostream>
#include "../utils/mpl.hpp"

namespace textip {
namespace trie_impl_ {

template <typename Node, bool Const>
class trie_iterator : public std::iterator<std::forward_iterator_tag, constify<typename Node::value_type, Const>> {
public:
  typedef constify<Node, Const>* node_p;
  friend class trie_iterator<Node, true>;
  trie_iterator(trie_iterator<Node, false> const& other) : trie_iterator(other.node_) {}
  trie_iterator() {}
  trie_iterator(node_p node) : node_(next_valid_node_(node)) {

  }

  typename trie_iterator::reference operator* () const {
    return *node_->value_;
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
private:
  // Find next node with a value (or itself if already valid)
  static node_p next_valid_node_(node_p node) {
    while (node && !node->value_) {
      node = next_(node);
    }
    return node;
  }
  // Immediate next node or null
  static node_p next_(node_p node) {
    node_p first_child = node->first_child();
    if (first_child) {
      return first_child;
    }
    while (node) {
      node_p adjacent = node->next_child();
      if (adjacent) {
        return adjacent;
      }
      node = node->parent();
    }
    return nullptr;
  }
  node_p node_ = nullptr;
};
}
}

#endif
