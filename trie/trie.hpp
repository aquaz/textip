#ifndef TEXTIP_TRIE_TRIE_H
#define TEXTIP_TRIE_TRIE_H

#include "key_traits.hpp"
#include "trie_iterator.hpp"
#include "simple_node.hpp"

namespace textip {
namespace trie_impl_ {
template <typename Key, typename Mapped, template <typename, typename> class NodeImpl, typename KeyTraits = key_traits<Key>>
class trie {
public:
  trie() {}
  template <typename InputIt>
  trie(InputIt first, InputIt last) {
    insert(first, last);
  }
  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    for (; first != last; ++first) {
      insert(*first);
    }
  }
  typedef Key key_type;
  typedef std::pair<const Key, Mapped> value_type;
  typedef NodeImpl<KeyTraits, value_type> node_t;
  typedef trie_iterator<node_t, true> const_iterator;
  typedef trie_iterator<node_t, false> iterator;
  std::pair<iterator, bool> insert(value_type const& value) {
    Key const& key = value.first;
    auto it = key.begin();
    auto end = key.end();
    node_t* node = &root_;
    while (it != end) {
      std::tie(it, node) = node->make_child(it, end);
    }
    if (node->value_) {
      return { iterator(node), false };
    }
    ++size_;
    node->value_ = boost::in_place(value);
    return { iterator(node), true };
  }
  Mapped& operator[](Key const& key) {
    return insert(value_type(key, Mapped())).first->second;
  }
  const_iterator find(Key const& key) const {
    auto it = key.begin();
    auto end = key.end();
    node_t const* node = &root_;
    while (it != end) {
      std::tie(it, node) = node->find_child(it, end);
      if (node == nullptr) {
        return iterator();
      }
    }
    return const_iterator(node->value_ ? node : nullptr);
  }
  const_iterator begin() const {
    return const_iterator(&root_);
  }
  iterator begin() {
    return iterator(&root_);
  }
  const_iterator end() const {
    return const_iterator();
  }
  iterator end() {
    return iterator();
  }

  /// Number of keys
  std::size_t size() const {
    return size_;
  }
private:
  node_t root_ {nullptr};
  std::size_t size_ = 0;
};
}

template <typename K, typename V>
using trie = trie_impl_::trie<K, V, trie_impl_::simple_node>;

}

#endif /* !TEXTIP_TRIE_TRIE_H */
