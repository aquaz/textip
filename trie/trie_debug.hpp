#ifndef TEXTIP_TRIE_TRIE_DEBUG_H
#define TEXTIP_TRIE_TRIE_DEBUG_H

#include <iosfwd>
#include <string>

#include "trie.hpp"

namespace textip {
template <typename Key, typename T>
class TrieDebug {
private:
  typedef Trie<Key, T> trie_t;
public:
  TrieDebug(trie_t const& t) : t_(t) {
  }

  friend std::ostream& operator<< (std::ostream& ostr, TrieDebug && td) {
    td.print_(ostr);
    return ostr;
  }
private:
  void print_(std::ostream& ostr) {
    printNode_(t_.root_, ostr);
  }
  void printNode_(typename trie_t::Node const& node, std::ostream& ostr) {
    ostr << tab_;
    ostr << std::string(tab_, '.');
    if (node.parent_)
      ostr << node.c_ << " ";
    ostr << "(" << &node.parent_ << ") " << &node << " ";
    if (node.val_) {
      ostr << node.val_->second;
    }
    ostr << std::endl;
    ++tab_;
    for (auto & child : node.childs_) {
      printNode_(child, ostr);
    }
    --tab_;
  }

  int tab_ = 0;
  trie_t const& t_;
};

template <typename Key, typename T>
TrieDebug<Key, T> trie_debug(Trie<Key, T> const& t) {
  return TrieDebug<Key, T> (t);
}
}

#endif
