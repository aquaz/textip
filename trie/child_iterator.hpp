#ifndef TEXTIP_TRIE_CHILD_ITERATOR
#define TEXTIP_TRIE_CHILD_ITERATOR

#include <iterator>

#include <boost/range.hpp>

namespace textip {
namespace trie_impl_ {

template <typename Trie, typename Node>
class child_iterator : public std::iterator<std::forward_iterator_tag, Node> {
public:
  child_iterator(Trie const& trie, Node* node) : trie_(&trie), node_(node) {

  }
  child_iterator() {}
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
    node_ = node_->next_child(*trie_);
    return *this;
  }
private:
  Trie const* trie_ = nullptr;
  Node* node_ = nullptr;
};

// Returns child range of node
template <typename Trie, typename Node>
auto childs_range(Trie const& trie, Node& node) {
  return boost::make_iterator_range(child_iterator<Trie, Node>(trie, node.first_child(trie)),
                                    child_iterator<Trie, Node>());
}
}
}

#endif /* !TEXTIP_TRIE_CHILD_ITERATOR */
