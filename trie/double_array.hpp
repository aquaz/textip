#ifndef TEXTIP_TRIE_DOUBLE_ARRAY
#define TEXTIP_TRIE_DOUBLE_ARRAY

#include <memory>
#include <vector>

#include "../utils/class_helpers.hpp"
#include "child_iterator.hpp"

namespace textip {
namespace trie_impl_ {

template <typename KeyTraits, typename Value>
class double_array {
  class node_structure_;
public:
  typedef node_structure_ node_t;
  double_array() {
    grow_(1 + root_position + KeyTraits::mapped_range);
  }
  static constexpr std::size_t root_position = 1;
  node_t const* root() const { return &array_[root_position]; }
  NON_CONST_GETTER(root)
private:
  std::vector<node_t> array_;

  std::size_t grow_() { return grow_(array_.size()); }
  std::size_t grow_(std::size_t size) {
    std::size_t last_size = array_.size();
    array_.resize(last_size + size);
    for (auto it = array_.begin() + last_size; it != array_.end(); ++it) {
      it->trie_ = this;
    }
    return last_size;
  }

  class node_structure_ {
  public:
    friend class double_array;
    typedef node_structure_ this_t;
    typedef Value value_type;
    typedef typename KeyTraits::iterator char_iterator;
    typedef typename KeyTraits::char_type char_type;
    typedef typename KeyTraits::char_mapped_type char_mapped_type;
    typedef std::make_signed_t<char_mapped_type> char_offset_type;

    std::pair<char_iterator, this_t const*> find_child(char_iterator begin, char_iterator) const {
      this_t const& child = child_map_(*begin);
      return { begin + 1, is_child_(child) ? &child : nullptr };
    }

    struct insert_finder_ {
      insert_finder_(std::size_t index, char_mapped_type target) : index(index), target(target), prev_char(target) {}
      bool operator()(this_t const& node) {
        char_mapped_type c = node.position_() - index;
        if (c < target && (c + node.next_ > target || node.next_ == 0)) {
          prev_char = c;
          return true;
        }
        return false;
      }
      std::size_t index;
      char_mapped_type target;
      char_mapped_type prev_char;
    };
    // Find or create child matching *it
    std::pair<char_iterator, this_t*> make_child(char_iterator begin, char_iterator) {
      char_mapped_type child_char = KeyTraits::map_char(*begin);
      {
        this_t& child = child_(child_char);
        if (is_child_(child) || child.is_free_()) {
          if (child.is_free_()) {
            insert_finder_ finder(index_, child_char);
            for (this_t const& node : childs_range(*this)) {
              if (finder(node))
                break;
            }
            insert_child_(child_char, finder.prev_char);
          }
          return { begin + 1, &child };
        }
      }
      this_t* new_this;
      std::size_t new_index;
      std::tie(new_this, new_index) = find_new_index_(child_char);
      // From now *this may be invalidated, it must not be accessed
      new_this->relocate_(new_index, child_char);
      return { begin + 1, &new_this->child_(child_char) };
    }

    // Relocate childs and insert new child
    void relocate_(std::size_t new_index, char_mapped_type child_char) {
      insert_finder_ finder(index_, child_char);
      for (this_t& node : childs_range(*this)) {
        char_mapped_type c = node.position_() - index_;
        finder(node);
        std::size_t new_node_pos = new_index + c;
        this_t& new_node = node_at_(new_node_pos);
        for (this_t& child_node : childs_range(node)) {
          child_node.parent_ = new_node_pos; // Update parent of moved child
        }
        std::swap(new_node.parent_, node.parent_);
        new_node.index_ = node.index_;
        new_node.first_child_ = node.first_child_;
        new_node.next_ = node.next_;
        new_node.value = std::move(node.value);
      }
      index_ = new_index;
      insert_child_(child_char, finder.prev_char);
    }
    // Insert child at child_char after child at prev_child_char.
    // If child_char == prev_child_char, insert as first child
    void insert_child_(char_mapped_type child_char, char_mapped_type prev_child_char) {
      this_t& new_child = child_(child_char);
      new_child.parent_ = position_();
      new_child.index_ = index_;
      char_offset_type offset_to_child = child_char - prev_child_char;
      if (offset_to_child) {
        char_offset_type& prev_next = child_(prev_child_char).next_;
        new_child.next_ = prev_next ? prev_next - offset_to_child : 0;
        prev_next = offset_to_child;
      } else {
        new_child.next_ = first_child() ? first_child_ - child_char : 0;
        first_child_ = child_char;
      }
    }

    void remove_child(this_t const*) {
      //throw "not yet implemeted";
    }

    this_t const* first_child() const {
      this_t const& child = child_(first_child_);
      if (!is_child_(child))
        return nullptr;
      return &child;
    }
    NON_CONST_GETTER(first_child)

    this_t const* next_child() const {
      if (0 == next_)
        return nullptr;
      return &node_at_(position_() + next_);
    }
    NON_CONST_GETTER(next_child)

    this_t const* parent() const {
      if (position_() == double_array::root_position)
        return nullptr; // this is root node
      return &node_at_(parent_);
    }
    NON_CONST_GETTER(parent)

    std::unique_ptr<Value> value;
  private:
    // Find index such that every childs and new childs are free
    // This may invalidate this pointer so the new this is also returned
    std::pair<this_t*, std::size_t> find_new_index_(char_mapped_type new_c) {
      auto& array = trie_->array_;
      auto childs = childs_range(*this);
      for (std::size_t index = double_array::root_position + 1; index + KeyTraits::mapped_range - 1 < array.size(); ++index) {
        if (std::all_of(childs.begin(), childs.end(), [this, index](this_t const& node) {
        return node_at_(index + node.position_() - index_).is_free_();
        }) && node_at_(index + new_c).is_free_())
        return { this, index };
      }
      std::size_t pos = position_(); // save current position because *this may be invalidated
      double_array* trie = trie_;
      std::size_t index = trie->grow_();
      return { &trie->array_[pos], index };
    }
    // Tell if a node is currently in use. This method must not be called on root node
    bool is_free_() const {
      return parent_ == 0;
    }
    // Fetch child without checking validity
    this_t& child_map_(char_type c) const { return child_(KeyTraits::map_char(c)); }
    this_t& child_(char_mapped_type c) const { return node_at_(index_ + c); }
    // Check if this node is a child
    bool is_child_(this_t const& node) const { return node.parent_ == position_(); }
    // Position of this node in array
    std::size_t position_() const { return this - &trie_->array_.front(); }
    // Fetch node at postion
    this_t& node_at_(std::size_t pos) const { return trie_->array_[pos]; }
    // Global array pointer
    double_array* trie_ = nullptr;
    // Position of parent node. Unused nodes and root have it set to 0
    std::size_t parent_ = 0;
    // Start index for child indexing
    std::size_t index_ = double_array::root_position + 1;
    // Offset to first child
    char_mapped_type first_child_ = 0;
    // Offset to next sibling or 0 if this is the last child
    char_offset_type next_ = 0;
  };
};
}
}

#endif /* !TEXTIP_TRIE_DOUBLE_ARRAY */
