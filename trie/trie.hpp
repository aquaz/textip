#ifndef TEXTIP_TRIE_TRIE_H
#define TEXTIP_TRIE_TRIE_H

#include <vector>

#include <boost/optional.hpp>
#include "../utils/mpl.hpp"

namespace textip {

template <typename Key, typename T>
class TrieDebug;

template<typename Key, typename T>
class Trie {
public:
  Trie() {}
  template <typename InputIt>
  Trie ( InputIt first, InputIt last ) {
    std::for_each ( first, last, [this] ( value_type it ) {
      insert ( it );
    } );
  }

  T& operator[] ( Key const& key ) {
    return insert ( { key, T() } ).first->second;
  }

  typedef Key key_type;
  typedef std::pair<Key, T> value_type;
  typedef typename Key::value_type char_type;
private:
  friend class TrieDebug<Key, T>;
  struct Node {
    Node() {}
    Node ( Node&& other ) {
      *this = std::move ( other );
    }
    Node& operator= ( Node&& other ) {
      if ( this != &other ) {
        parent_ = other.parent_;
        std::swap ( val_, other.val_ );
        std::swap ( c_, other.c_ );
        std::swap ( childs_, other.childs_ );
        for ( auto& child : childs_ ) {
          child.parent_ = this;
        }
      }
      return *this;
    }
    Node ( Node const& other ) = delete;
    Node& operator= ( Node const& other ) = delete;
    Node ( Node* parent, char_type c )
      : parent_ ( parent ), c_ ( c ) {
    }
    Node* parent_ = nullptr;
    boost::optional<value_type> val_;
    char_type c_ = char_type();
    typedef std::vector<Node> nodes_t;
    nodes_t childs_;

    struct node_finder {
      bool operator() ( Node const& node, char_type const& c ) {
        return node.c_ < c;
      }
    };
    typename nodes_t::const_iterator find ( char_type const& c ) const {
      return std::lower_bound ( childs_.begin(), childs_.end(), c,
                                node_finder() );
    }

    typename nodes_t::iterator find ( char_type const& c ) {
      return std::lower_bound ( childs_.begin(), childs_.end(), c,
                                node_finder() );
    }

    bool node_is_char ( typename nodes_t::const_iterator it, char_type const& c ) const {
      return it != childs_.end() && it->c_ == c;
    }
    Node const* find_node ( char_type const& c ) const {
      auto it = find ( c );
      return node_is_char ( it, c ) ? &*it : nullptr;
    }

    Node* find_make_node ( char_type const& c ) {
      auto it = find ( c );
      if ( node_is_char ( it, c ) )
        return &*it;
      return &*childs_.insert ( it, Node ( this, c ) );
    }
  };

  template <bool Const>
  class Iterator : std::iterator<std::forward_iterator_tag, constify<value_type, Const>> {
  private:
    typedef constify<Node, Const> node_t;
  public:
    Iterator() {}

    typename Iterator::reference operator* () {
      return *node_->val_;
    }

    typename Iterator::pointer operator->() {
      return &**this;
    }

    bool operator!= ( Iterator const& other ) {
      return node_ != other.node_;
    }

    Iterator& operator++ () {
      while ( ( node_ = advance_ ( node_ ) ) && !node_->val_ )
        ;
      return *this;
    }
  private:
    friend class Trie;
    Iterator ( node_t* node )
      : node_ ( node ) {
      if ( node && !node->val_ )
        ++*this;
    }

    static node_t* advance_ ( node_t* node ) {
      if ( !node->childs_.empty() ) {
        return &node->childs_.front();
      }
      while ( 1 ) {
        node_t* parent = node->parent_;
        if ( parent == nullptr ) {
          return nullptr;
        }

        auto& siblings = parent->childs_;
        std::ptrdiff_t offset = node - &siblings.front();
        if ( offset + 1 < siblings.size() ) {
          return &siblings[offset + 1];
        }
        node = parent;
      }
    }
    node_t* node_ = nullptr;
  };
public:
  typedef Iterator<true> const_iterator;
  typedef Iterator<false> iterator;

  std::pair<iterator, bool> insert ( value_type const value ) {
    Node* node = &root_;
    for ( auto& c : value.first ) {
      node = node->find_make_node ( c );
    }
    if ( node->val_ )
      return { iterator ( node ), false };
    node->val_ = value;
    return { iterator ( node ), true };
  }

  const_iterator find ( Key const& key ) const {
    Node const* node = &root_;
    for ( auto& c : key ) {
      node = node->find_node ( c );
      if ( !node ) {
        return const_iterator();
      }
    }
    if ( node->val_ ) {
      return const_iterator ( node );
    }
    return const_iterator();
  }

  iterator find ( Key const& key ) {
    return iterator ( const_cast<Node*> ( static_cast<const Trie*> ( this )->find ( key ).node_ ) );
  }

  const_iterator begin() const {
    return const_iterator ( &root_ );
  }
  iterator begin() {
    return iterator ( &root_ );
  }
  const_iterator end() const {
    return const_iterator();
  }
  iterator end() {
    return iterator();
  }

private:

  Node root_;
};

}

#endif /* !TEXTIP_TRIE_TRIE_H */
