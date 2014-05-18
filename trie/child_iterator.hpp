#ifndef TEXTIP_TRIE_CHILD_ITERATOR
#define TEXTIP_TRIE_CHILD_ITERATOR

#include <iterator>

#include <boost/range.hpp>

namespace textip {
namespace trie_impl_ {

template <typename Node>
class child_iterator : public std::iterator<std::forward_iterator_tag, Node> {
public:
  child_iterator(Node* node = nullptr) : node_(node) {

  }
  Node& operator* () const {
    return *node_;
  }
  Node* operator->() const {
    return node_;
  }
  bool operator!= (child_iterator const& other) const {
    return node_ != other.node_;
  }
  bool operator== (child_iterator const& other) const {
    return node_ == other.node_;
  }
  child_iterator& operator++() {
    node_ = node_->next_child();
    return *this;
  }
private:
  Node* node_ = nullptr;
};

// Returns child range of node
template <typename Node>
auto childs_range(Node&& node) {
  typedef std::remove_reference_t<Node> node_t;
  return boost::make_iterator_range(child_iterator<node_t>(node.first_child()), child_iterator<node_t>());
}
}
}

#endif /* !TEXTIP_TRIE_CHILD_ITERATOR */
