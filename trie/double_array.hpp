#ifndef TEXTIP_TRIE_DOUBLE_ARRAY
#define TEXTIP_TRIE_DOUBLE_ARRAY

#include <boost/range/algorithm.hpp>

#include <cassert>
#include <memory>
#include <vector>

#ifndef NDEBUG
#include <unordered_set>
#endif

#ifndef RELEASE
#define profile_noinline __attribute__((noinline))
#else
#define profile_noinline
#endif

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
    array_.front().unused.prev_ = array_.size() - 1;
    node_t& r = *root();
    r.remove_from_free_list_(*this);
    r.used.index_ = root_position + 1;
    r.used.first_child_ = 0;
    assert_free_list_();
  }
  static constexpr std::size_t root_position = 1;
  node_t const* root() const {return &array_[root_position]; }
  NON_CONST_GETTER(root)
private:
  mutable std::vector<node_t> array_; // TODO Remove mutable

  node_t& node_at_(std::size_t pos) const { return array_[pos]; }
#ifndef NDEBUG
  bool valid_node_(node_t const* node) const {
    return array_.size() && node >= &array_.front() && node <= &array_.back();
  }
#endif
  void assert_free_list_() const {
#ifndef NDEBUG
    std::size_t n = 0;
    std::unordered_set<std::size_t> v;
    do {
      node_t& node = array_[n];
      assert(node.parent_ == 0);
      assert(array_[node.unused.prev_].unused.next_ == n);
      v.insert(n);
      n = node.unused.next_;
    } while (n);
    for (auto it = array_.begin() + 2; it != array_.end(); ++it) {
      assert(it->parent_ != 0 || v.find(it->position_(*this)) != v.end());
    }
#endif
  }
  std::size_t grow_() {
    std::size_t last_size = grow_(array_.size());
    std::size_t& first_prev = array_.front().unused.prev_;
    array_[last_size].unused.prev_ = first_prev;
    array_[first_prev].unused.next_ = last_size;
    first_prev = array_.size() - 1;
    assert_free_list_();
    return last_size;
  }
  std::size_t grow_(std::size_t size) {
    std::size_t last_size = array_.size();
    array_.resize(last_size + size);
    std::size_t position = last_size;
    for (auto it = array_.begin() + last_size; it != array_.end(); ++it) {
      it->unused.next_ = position + 1;
      it->unused.prev_ = position - 1;
      ++position;
    }
    array_.back().unused.next_ = 0;
    return last_size;
  }
  template <bool Const>
  class free_list_iterator_impl : public std::iterator<std::bidirectional_iterator_tag, constify<node_t, Const>> {
  public:
    typedef typename free_list_iterator_impl::reference reference;
    typedef typename free_list_iterator_impl::pointer pointer;
    free_list_iterator_impl(double_array const& trie, reference node) : trie_(&trie), node_(&node) {}
    free_list_iterator_impl& operator++ () {
      node_ = &node_->next_free_(*trie_);
      return *this;
    }
    free_list_iterator_impl operator++ (int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
    free_list_iterator_impl& operator-- () {
      node_ = &node_->prev_free_(*trie_);
      return *this;
    }
    free_list_iterator_impl operator-- (int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }
    // Return non const iterator
    auto remove_const() const { return free_list_iterator_impl<false>(*node_); }
    bool operator== (free_list_iterator_impl const& other) const {
      return node_ == other.node_;
    }
    bool operator!= (free_list_iterator_impl const& other) const {
      return node_ != other.node_;
    }
    reference operator* () const {
      return *node_;
    }
    pointer operator-> () const {
      return node_;
    }
  private:
    double_array const* trie_;
    pointer node_;
  };
  typedef free_list_iterator_impl<true> free_list_iterator_const;
  typedef free_list_iterator_impl<false> free_list_iterator;
  // Sentinel free node
  auto end_free_node() const { return free_list_iterator_const(*this, array_[0]); }
  auto free_list_range() const {
    auto end = end_free_node();
    return boost::make_iterator_range(
             free_list_iterator_const(*this, end->next_free_(*this)), end);
  }

  class node_structure_ {
  public:
    friend class double_array;
    typedef node_structure_ this_t;
    typedef Value value_type;
    typedef typename KeyTraits::iterator char_iterator;
    typedef typename KeyTraits::char_type char_type;
    typedef typename KeyTraits::char_mapped_type char_mapped_type;
    typedef char_mapped_type char_offset_type;

    std::pair<char_iterator, this_t const*> find_child(double_array const& trie, char_iterator begin, char_iterator) const profile_noinline {
      this_t const& child = child_map_(trie, *begin);
      return { begin + 1, is_child_(trie, child) ? &child : nullptr };
    }

    struct insert_finder_ {
      insert_finder_(double_array const& trie, std::size_t index, char_mapped_type target) : trie(trie), index(index), target(target), prev_char(target) {}
      bool operator()(this_t const& node) {
        char_mapped_type c = node.position_(trie) - index;
        if (c < target && (c + node.used.next_ > target || node.used.next_ == 0)) {
          prev_char = c;
          return true;
        }
        return false;
      }
      double_array const& trie;
      std::size_t index;
      char_mapped_type target;
      char_mapped_type prev_char;
    };
    // Find or create child matching *it
    std::pair<char_iterator, this_t*> make_child(double_array& trie, char_iterator begin, char_iterator) profile_noinline {
      char_mapped_type child_char = KeyTraits::map_char(*begin);
      {
        this_t& child = child_(trie, child_char);
        if (is_child_(trie, child) || child.is_free_()) {
          if (child.is_free_()) {
            insert_finder_ finder(trie, used.index_, child_char);
            for (this_t const& node : childs_range(trie, *this)) {
              if (finder(node))
                break;
            }
            insert_child_(trie, child_char, finder.prev_char);
          }
          assert(trie.valid_node_(&child));
          return { begin + 1, &child };
        }
      }
      this_t* new_this;
      std::size_t new_index;
      std::tie(new_this, new_index) = find_new_index_(trie, child_char);
      // From now *this may be invalidated, it must not be accessed
      new_this->relocate_(trie, new_index, child_char);
      assert(trie.valid_node_(&new_this->child_(trie, child_char)));
      return { begin + 1, &new_this->child_(trie, child_char) };
    }

    // Relocate childs and insert new child
    void relocate_(double_array const& trie, std::size_t new_index, char_mapped_type child_char) profile_noinline {
      insert_finder_ finder(trie, used.index_, child_char);
      std::size_t free_list_hint = 0;
      for (this_t* node = first_child(trie); node;) {
        std::size_t node_position = node->position_(trie);
        char_mapped_type c = node_position - used.index_;
        finder(*node);
        std::size_t new_node_pos = new_index + c;
        this_t& new_node = trie.node_at_(new_node_pos);
        for (this_t& child_node : childs_range(trie, *node)) {
          child_node.parent_ = new_node_pos; // Update parent of moved child
        }
        this_t* next_node = node->next_child(trie); // Save next child because node will be invalid
        new_node.remove_from_free_list_(trie);
        std::swap(new_node.parent_, node->parent_);
        new_node.used.index_ = node->used.index_;
        new_node.used.first_child_ = node->used.first_child_;
        new_node.used.next_ = node->used.next_;
        new_node.value = std::move(node->value);
        node->add_to_free_list_(trie, free_list_hint);
        free_list_hint = node_position;
        assert(new_node.parent_ != 0);
        assert(node->parent_ == 0);
        node = next_node;
      }
      used.index_ = new_index;
      insert_child_(trie, child_char, finder.prev_char);
    }

    // Find first freenode after position, starting at free node at hint
    std::size_t find_free_node_(double_array const& trie, std::size_t hint) const profile_noinline {
      if (!hint) {
        hint = trie.node_at_(0).next_free_(trie).position_(trie);
      }
      this_t const& start = trie.node_at_(hint);
      assert(start.is_free_());
      return std::find_if(free_list_iterator_const(trie, start), trie.end_free_node(),
      [this](this_t const& node) { return &node > this; }
                         )->position_(trie);
    }
    void add_to_free_list_(double_array const& trie, std::size_t hint) profile_noinline {
      std::size_t position = position_(trie);
      std::size_t n = find_free_node_(trie, hint);
      this_t& next = trie.node_at_(n);
      unused.next_ = n;
      unused.prev_ = next.unused.prev_;
      next.unused.prev_ = position;
      trie.node_at_(unused.prev_).unused.next_ = position;
      assert(trie.node_at_(unused.prev_).parent_ == 0);
      trie.assert_free_list_();
    }
    void remove_from_free_list_(double_array const& trie)  {
      trie.assert_free_list_();
      assert(parent_ == 0);
      assert(trie.node_at_(unused.next_).parent_ == 0);
      assert(trie.node_at_(unused.prev_).parent_ == 0);
      next_free_(trie).unused.prev_ = unused.prev_;
      prev_free_(trie).unused.next_ = unused.next_;
    }
    // Insert child at child_char after child at prev_child_char.
    // If child_char == prev_child_char, insert as first child
    void insert_child_(double_array const& trie, char_mapped_type child_char, char_mapped_type prev_child_char)  {
      check_childs(trie);
      this_t& new_child = child_(trie, child_char);
      assert(trie.valid_node_(&new_child));
      new_child.remove_from_free_list_(trie);
      new_child.parent_ = position_(trie);
      trie.assert_free_list_();
      new_child.used.index_ = used.index_;
      new_child.used.first_child_ = 0;
      char_offset_type offset_to_child = child_char - prev_child_char;
      if (offset_to_child) {
        char_offset_type& prev_next = child_(trie, prev_child_char).used.next_;
        new_child.used.next_ = prev_next ? prev_next - offset_to_child : 0;
        prev_next = offset_to_child;
      } else {
        new_child.used.next_ = first_child(trie) ? used.first_child_ - child_char : 0;
        used.first_child_ = child_char;
      }
      check_childs(trie);
    }

#ifndef NDEBUG
    void check_childs(double_array const& trie) const {
      for (this_t const& child : childs_range(trie, *this)) {
        assert(child.parent_);
        assert(trie.valid_node_(&child));
      }
    }
#else
    void check_childs(double_array const&) const {

    }
#endif

    void remove_child(double_array const&, this_t const*) {
      //throw "not yet implemeted";
    }

    this_t const* first_child(double_array const& trie) const {
      this_t const& child = child_(trie, used.first_child_);
      if (!is_child_(trie, child))
        return nullptr;
      return &child;
    }
    NON_CONST_GETTER(first_child)

    this_t const* next_child(double_array const&) const {
      if (0 == used.next_)
        return nullptr;
      return this + used.next_;
    }
    NON_CONST_GETTER(next_child)

    this_t const* parent(double_array const& trie) const {
      if (position_(trie) == root_position)
        return nullptr; // this is root node
      return &trie.node_at_(parent_);
    }
    NON_CONST_GETTER(parent)

    std::unique_ptr<Value> value;
  private:
    // Find index such that every childs and new childs are free
    // This may invalidate this pointer so the new this is also returned
    std::pair<this_t*, std::size_t> find_new_index_(double_array& trie, char_mapped_type new_c) profile_noinline {
      assert(trie.valid_node_(this));
      auto& array = trie.array_;
      auto free_end = trie.end_free_node();
      auto f = boost::range::find_if(trie.free_list_range(), [new_c, &trie](this_t const& node)
      { return node.position_(trie) > root_position + new_c; });
      f = std::find_if(f, free_end, [this, childs = childs_range(trie, *this),new_c, array_size = array.size(), &trie](this_t const& node) {
        std::size_t index = node.position_(trie) - new_c;
        return index + KeyTraits::mapped_range - 1 < array_size
        && std::all_of(childs.begin(), childs.end(),
        [this, index, &trie](this_t const& node) {
          return trie.node_at_(index + (node.position_(trie) - used.index_)).is_free_();
        });
      });
      if (f != free_end) {
        return { this, f->position_(trie) - new_c };
      }
      std::size_t pos = position_(trie); // save current position because *this may be invalidated
      std::size_t index = trie.grow_();
      return { &array[pos], index };
    }
    // Get next free node
    this_t& next_free_(double_array const& trie) const {
      assert(is_free_());
      return trie.node_at_(unused.next_);
    }
    // Get previous free node
    this_t& prev_free_(double_array const& trie) const {
      assert(is_free_());
      return trie.node_at_(unused.prev_);
    }
    // Tell if a node is currently in use. This method must not be called on root node
    bool is_free_() const {
      return parent_ == 0;
    }
    // Fetch child without checking validity
    this_t& child_map_(double_array const& trie, char_type c) const { return child_(trie, KeyTraits::map_char(c)); }
    this_t& child_(double_array const& trie, char_mapped_type c) const { return trie.node_at_(used.index_ + c); }
    // Check if this node is a child
    bool is_child_(double_array const& trie, this_t const& node) const { return node.parent_ == position_(trie); }
    // Position of this node in array
    std::size_t position_(double_array const& trie) const { return this - &trie.array_.front(); }
    // Position of parent node. Unused nodes and root have it set to 0
    std::size_t parent_ = 0;
    union {
      // Lookup data for used node
      struct {
        // Start index for child indexing
        std::size_t index_;
        // Offset to first child
        char_mapped_type first_child_;
        // Offset to next sibling or 0 if this is the last child
        char_offset_type next_;
      } used;
      // Doubly linked list for unused nodes
      struct {
        std::size_t next_; // Next free node
        std::size_t prev_; // Previous free node
      } unused;
    };
  };
};
}
}

#endif /* !TEXTIP_TRIE_DOUBLE_ARRAY */

