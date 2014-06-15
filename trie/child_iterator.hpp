#ifndef TEXTIP_TRIE_CHILD_ITERATOR
#define TEXTIP_TRIE_CHILD_ITERATOR

#include <iterator>

#include <boost/range.hpp>

namespace textip {
namespace trie_impl_ {

template <typename Trie>
class child_iterator : public std::iterator<std::forward_iterator_tag, typename Trie::node_t> {
public:
  typedef typename Trie::node_t node_t;
  child_iterator(Trie const& trie, node_t* node) : trie_(&trie), node_(node) {

  }
  child_iterator() {}
  node_t& operator* () const {
    return *node_;
  }
  node_t* operator->() const {
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
  node_t* node_ = nullptr;
};

// Returns child range of node
template <typename Trie>
auto childs_range(Trie const& trie, typename Trie::node_t& node) {
  return boost::make_iterator_range(child_iterator<Trie>(trie, node.first_child(trie)), child_iterator<Trie>());
}
}
}

#endif /* !TEXTIP_TRIE_CHILD_ITERATOR */
