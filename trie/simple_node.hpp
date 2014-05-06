#ifndef TEXTIP_TRIE_SIMPLE_NODE_H
#define TEXTIP_TRIE_SIMPLE_NODE_H

#include <algorithm>
#include <vector>

#include <boost/optional.hpp>

namespace textip {
namespace trie_impl_ {

template <typename KeyTraits, typename Value>
class simple_node {
public:
  typedef Value value_type;
  typedef typename KeyTraits::iterator char_iterator;
  typedef typename KeyTraits::char_type char_type;
  simple_node ( simple_node const& other ) = delete;
  simple_node& operator= ( simple_node const& other ) = delete;
  simple_node ( simple_node&& other ) {
    *this = std::move ( other );
  }
  simple_node& operator= ( simple_node&& other ) {
    if ( this != &other ) {
      parent_ = other.parent_;
      value_ = other.value_;
      c_ = other.c_;
      childs_ = std::move ( other.childs_ );
      for ( auto& n : childs_ ) {
        n.parent_ = this;
      }
    }
    return *this;
  }
  simple_node ( simple_node* parent, char_type c = char_type() ) : parent_ ( parent ), c_ ( c ) {
  }
  struct node_finder {
    bool operator() ( simple_node const& node, char_type c ) {
      return node.c_ < c;
    }
  };
  // Find child matching begining of [it,end)
  std::pair<char_iterator, simple_node*> find_child ( char_iterator begin, char_iterator ) {
    auto it = std::lower_bound ( childs_.begin(), childs_.end(), *begin, node_finder() );
    if ( it == childs_.end() || it->c_ != *begin ) {
      return { begin, nullptr };
    }
    return { begin + 1, &*it };
  }
  std::pair<char_iterator, simple_node*> make_child ( char_iterator begin, char_iterator ) {
    auto it = std::lower_bound ( childs_.begin(), childs_.end(), *begin, node_finder() );
    if ( it != childs_.end() && it->c_ == *begin ) {
      return { begin + 1, &*it};
    }
    return { begin + 1, &*childs_.insert ( it, simple_node ( this, *begin ) ) };
  }

  simple_node* first_child() {
    return &childs_.front();
  }

  simple_node* next_child() {
    if ( this->parent() == nullptr ) {
      return nullptr;
    }
    auto& siblings = this->parent()->childs_;
    std::size_t pos = this - first_child() + 1;
    return pos < siblings.size() ? &siblings[pos] : nullptr;
  }

  boost::optional<Value> value_;
  char_type c() const {
    return c_;
  };

  simple_node* parent() {
    return parent_;
  }
  simple_node const* parent() const {
    return parent_;
  }
private:
  simple_node* parent_;
  char_type c_;
  std::vector<simple_node> childs_;
};
}
}

#endif
