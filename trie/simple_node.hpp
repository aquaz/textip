#ifndef TEXTIP_TRIE_SIMPLE_NODE_H
#define TEXTIP_TRIE_SIMPLE_NODE_H

#include <algorithm>
#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "../utils/class_helpers.hpp"

namespace textip {
namespace trie_impl_ {

template <typename KeyTraits, typename Value>
class simple_node {
public:
  typedef Value value_type;
  typedef typename KeyTraits::iterator char_iterator;
  typedef typename KeyTraits::char_type char_type;
  typedef simple_node this_t;

  simple_node() {}
  simple_node(this_t const& other) = delete;
  simple_node& operator= (this_t const& other) = delete;
  simple_node(this_t&& other) {
    *this = std::move(other);
  }
  simple_node& operator= (this_t&& other) {
    if (this != &other) {
      parent_ = other.parent_;
      value = std::move(other.value);
      c_ = std::move(other.c_);
      childs_ = std::move(other.childs_);
      for (auto& n : childs_) {
        n.parent_ = this;
      }
    }
    return *this;
  }
  simple_node(this_t* parent, char_type c) : parent_(parent), c_(c) {
  }

  // Compare with character
  bool operator< (char_type c) const {
    return c_ < c;
  }
  // Find child matching *it
  std::pair<char_iterator, this_t const*> find_child(char_iterator begin, char_iterator) const {
    auto it = std::lower_bound(childs_.begin(), childs_.end(), *begin);
    if (it == childs_.end() || it->c_ != *begin) {
      return { begin, nullptr };
    }
    return { begin + 1, &*it };
  }
  // Find or create child matching *it
  std::pair<char_iterator, this_t*> make_child(char_iterator begin, char_iterator) {
    auto it = std::lower_bound(childs_.begin(), childs_.end(), *begin);
    if (it != childs_.end() && it->c_ == *begin) {
      return { begin + 1, &*it};
    }
    return { begin + 1, &*childs_.emplace(it, this, *begin) };
  }

  void remove_child(this_t const* node) {
    std::size_t pos = node - first_child();
    childs_.erase(childs_.begin() + pos);
  }

  this_t const* first_child() const {
    return &childs_.front();
  }
  NON_CONST_GETTER(first_child)

  this_t const* next_child() const {
    if (this->parent() == nullptr) {
      return nullptr;
    }
    auto& siblings = parent_->childs_;
    std::size_t pos = this - parent_->first_child() + 1;
    return pos < siblings.size() ? &siblings[pos] : nullptr;
  }
  NON_CONST_GETTER(next_child)

  char_type c() const {
    return c_;
  };

  this_t const* parent() const {
    return parent_;
  }
  NON_CONST_GETTER(parent)

  std::unique_ptr<Value> value;
private:
  this_t* parent_ = nullptr;
  char_type c_ {};
  std::vector<this_t> childs_;
};

}
}

#endif
