#ifndef TEXTIP_TRIE_NODE_PROXY_H
#define TEXTIP_TRIE_NODE_PROXY_H

namespace textip {
template <typename Trie, bool Const>
class trie_node_proxy {
public:
  typedef constify<Trie, Const> trie_t;
  typedef constify<typename Trie::node_t, Const> node_t;
  typedef typename Trie::key_traits::key_type key_type;
  typedef typename Trie::key_traits::char_type char_type;
  trie_node_proxy() {}
  trie_node_proxy(trie_t& trie, node_t* node)
    : trie_(&trie), node_(node) {

  }
  trie_node_proxy<Trie, false> remove_const() const {
    return trie_node_proxy<Trie, false>(const_cast<Trie&>(*trie_), const_cast<typename Trie::node_t*>(node_));
  }
  trie_node_proxy create(node_t* node) const {
    return trie_node_proxy(*trie_, node);
  }

  trie_node_proxy find(key_type const& key) const {
    return find(std::begin(key), std::end(key));
  }

  template <typename CharIt>
  trie_node_proxy find(CharIt begin, CharIt end) const {
    node_t* n = node_;
    CharIt it = begin;
    while (it != end && n) {
      std::tie(it, n) = n->find_child(*trie_, it, end);
    }
    return trie_node_proxy(*trie_, n);
  }

  operator bool() const { return node_; }
  trie_node_proxy parent() const { return create(node_->parent(*trie_)); }
  trie_node_proxy first_child() const { return create(node_->first_child(*trie_)); }
  trie_node_proxy next_child() const { return create(node_->next_child(*trie_)); }
  auto& value() const { return node_->value; }
  node_t* ptr() const { return node_; }

  // standard trie takes no argument, particia trie takes one
  template <typename ...Args>
  char_type c(Args... args) const {
    return node_->c(trie_, args...);
  }
  std::size_t clen() const {
    return node_->clen(trie_);
  }
private:
  trie_t* trie_ = nullptr;
  node_t* node_ = nullptr;
};
}

#endif
