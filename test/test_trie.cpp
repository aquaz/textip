#define BOOST_TEST_MODULE trie test
#include <boost/test/unit_test.hpp>

#include "../trie/trie.hpp"
#include <unordered_map>

template <typename MapA, typename MapB>
void is_subset(MapA const& a, MapB const& b) {
  for (auto& p : a) {
    auto it = b.find(p.first);
    BOOST_CHECK(it != b.end() && *it == p);
  }
}

template <typename MapA, typename MapB>
void is_same(MapA const& a, MapB const& b) {
  BOOST_CHECK_EQUAL(a.size(), b.size());
  is_subset(a, b);
  is_subset(b, a);
}

#define apply_operation(op) \
template <typename MapA, typename MapB, typename ...Args> \
void apply_##op(MapA& a, MapB& b, Args const& ...args) { \
  a.op(args...); \
  b.op(args...);\
}

apply_operation(insert)

void test_scenarii(std::vector<std::string> const& v) {
  std::unordered_map<std::string, int> m;
  textip::trie<std::string, int> t;
  int value = 0;
  for (std::string const& s : v) {
    apply_insert(m, t, std::make_pair(s, ++value));
  }
  is_same(m, t);
}

BOOST_AUTO_TEST_CASE(associoative_container) {
  test_scenarii( {"tést", "test", "etre", "testera", "tester", "tertre", "atre", "etait", "était"});
}
