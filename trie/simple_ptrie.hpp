#ifndef TEXTIP_TRIE_SIMPLE_PTRIE_H
#define TEXTIP_TRIE_SIMPLE_PTRIE_H

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

#include "../utils/class_helpers.hpp"
#include "../utils/debug_tools.hpp"

namespace textip { namespace trie_impl_ {

template <typename KeyTraits, typename Value>
class simple_ptrie {
  typedef typename KeyTraits::iterator char_iterator;
  typedef typename KeyTraits::char_type char_type;
  typedef std::vector<char_type> chars_t;
  typedef simple_ptrie this_t;
public:
  typedef Value value_type;
  typedef this_t node_t;
  this_t const* root() const { return this; }
  NON_CONST_GETTER(root)
  
  simple_ptrie() {}
  simple_ptrie(this_t const& other) = delete;
  simple_ptrie& operator= (this_t const& other) = delete;
  simple_ptrie(this_t&& other) {
    *this = std::move(other);
  }
  simple_ptrie& operator= (this_t&& other) {
    if (this != &other) {
      parent_ = other.parent_;
      value = std::move(other.value);
      chars_ = std::move(other.chars_);
      childs_ = std::move(other.childs_);
      for (auto& n : childs_) {
        n.parent_ = this;
      }
    }
    return *this;
  }
  simple_ptrie(this_t* parent, char_iterator begin, char_iterator end) : parent_(parent), chars_(begin, end) {
    assert(chars_.size());
  }
  
  bool operator< (char_type c) const profile_noinline {
    return index_char_() < c;
  }
  std::pair<char_iterator, this_t const*> find_child(this_t const&, char_iterator begin, char_iterator end) const profile_noinline {
    assert(begin != end);
    auto it = std::lower_bound(childs_.begin(), childs_.end(), *begin);
    if (it == childs_.end())
      return { begin, nullptr };
    typename std::iterator_traits<char_iterator>::difference_type child_size = it->chars_.size();
    if (child_size > std::distance(begin, end) || !std::equal(it->chars_.begin(), it->chars_.end(), begin))
      return { begin, nullptr };
    return { begin + child_size, &*it };
  }
  
  std::pair<char_iterator, this_t*> make_child(this_t const& trie, char_iterator begin, char_iterator end) {
    assert(begin != end);
    auto child = std::lower_bound(childs_.begin(), childs_.end(), *begin);
    if (child == childs_.end() || child->index_char_() != *begin) {
      return { end, &*childs_.emplace(child, this_t(this, begin, end)) };
    }
    chars_t& chars = child->chars_;
    std::size_t minsize = std::min<std::size_t>(std::distance(begin, end), chars.size());
    char_iterator outer_it;
    typename chars_t::const_iterator chars_it;
    std::tie(outer_it, chars_it) = std::mismatch(begin, begin + minsize, chars.begin());
    assert(begin != outer_it);
    if (chars_it == chars.end()) {
      return { begin + minsize, &*child };
    }
    this_t node(this, begin, outer_it);
    chars.erase(chars.begin(), chars_it);
    node.childs_.emplace_back(std::move(*child));
    *child = std::move(node);
    if (outer_it == end) {
      return { end, &*child };
    }
    return child->make_child(trie, outer_it, end);
  }
  
  void remove_child(this_t const& trie, this_t const* node) {
    std::size_t pos = node - first_child(trie);
    childs_.erase(childs_.begin() + pos);
    if (childs_.size() == 1 && !value) {
      // fuse with child
      this_t& child = childs_.front();
      child.chars_.insert(child.chars_.begin(), chars_.begin(), chars_.end());
      child.parent_ = parent_;
      *this = std::move(child);
    }
  }
  
  this_t const* first_child(this_t const&) const {
    return childs_.size() ? &childs_.front() : nullptr;
  }
  NON_CONST_GETTER(first_child)

  this_t const* next_child(this_t const& t) const {
    this_t const* p = parent(t);
    if (p == nullptr || this == &p->childs_.back()) {
      return nullptr;
    }
    return this + 1;
  }
  NON_CONST_GETTER(next_child)

  this_t const* parent(this_t const& ) const {
    return parent_;
  }
  NON_CONST_GETTER(parent)
  
  std::unique_ptr<Value> value;
private:
  char_type index_char_() const {
    assert(chars_.size());
    return chars_[0];
  }
  
  this_t* parent_ = nullptr;
  chars_t chars_;
  std::vector<this_t> childs_;
};

}}

#endif /* TEXTIP_TRIE_SIMPLE_PTRIE_H */