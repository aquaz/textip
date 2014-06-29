#ifndef TEXTIP_TRIE_TRIE_H
#define TEXTIP_TRIE_TRIE_H

#include "key_traits.hpp"
#include "trie_iterator.hpp"
#include "simple_trie.hpp"
#include "simple_ptrie.hpp"
#include "double_array.hpp"

#include <initializer_list>

namespace textip {
namespace trie_impl_ {

template <typename Key, typename Mapped, template <typename, typename> class TrieImpl, typename KeyTraits = key_traits<Key>>
class trie {
public:
  typedef Key key_type;
  typedef std::pair<const Key, Mapped> value_type;
  typedef TrieImpl<KeyTraits, value_type> impl_t;
  typedef node_interface<impl_t, true> const_node_i;
  typedef node_interface<impl_t, false> node_i;
  typedef typename impl_t::node_t node_t;
  typedef trie_iterator<impl_t, true> const_iterator;
  typedef trie_iterator<impl_t, false> iterator;
  trie() {}
  trie(std::initializer_list<value_type> il) : trie(il.begin(), il.end()) {}
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
  auto insert(value_type const& value) {
    return insert(value_type(value));
  }
  std::pair<iterator, bool> insert(value_type&& value) {
    Key const& key = value.first;
    auto it = key.begin();
    auto end = key.end();
    node_t* node = impl_.root();
    while (it != end) {
      std::tie(it, node) = node->make_child(impl_, it, end);
    }
    if (node->value) {
      return { iterator(node_i(impl_, node)), false };
    }
    ++size_;
    node->value = std::make_unique<value_type>(std::move(value));
    return { iterator(node_i(impl_, node)), true };
  }
  Mapped& operator[](Key const& key) {
    return insert(value_type(key, Mapped())).first->second;
  }
  const_iterator find(Key const& key) const {
    auto node = root().find(key.begin(), key.end());
    return const_iterator((node && node.value()) ? node : nullptr);
  }
  iterator find(Key const& key) {
    return static_cast<trie const*>(this)->find(key).remove_const();
  }
  void erase(Key const& key) {
    auto it = find(key);
    if (it == end()) {
      return;
    }
    node_t* node = it.node().ptr();
    node->value = nullptr;
    while (!node->value && !node->first_child(impl_)) {
      node_t* p = node->parent(impl_);
      if (p == nullptr) {
        break;
      }
      p->remove_child(impl_, node);
      node = p;
    }
    --size_;
  }
  const_iterator begin() const {
    return const_iterator(root());
  }
  iterator begin() {
    return iterator(node_i(impl_, impl_.root()));
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

  const_node_i root() const { return const_node_i(impl_, impl_.root()); }
private:

  impl_t impl_;
  std::size_t size_ = 0;
};
}

template <typename K, typename V>
using trie = trie_impl_::trie<K, V, trie_impl_::simple_trie>;
template <typename K, typename V>
using ptrie = trie_impl_::trie<K, V, trie_impl_::simple_ptrie>;
template <typename K, typename V>
using datrie = trie_impl_::trie<K, V, trie_impl_::double_array>;

}

#endif /* !TEXTIP_TRIE_TRIE_H */
