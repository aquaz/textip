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

typedef std::unordered_map<std::string, int> reference_type;

template <typename T>
void is_same(reference_type const& a, T const& b) {
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
apply_operation(erase)

template <template<typename...> class T>
void test_scenarii(std::vector<std::string> const& v) {
  reference_type m;
  T<std::string, int> t;
  int value = 0;
  for (std::string const& s : v) {
    if (s.size() && s.front() == '-') {
      apply_erase(m, t, s.substr(1));
    } else {
      apply_insert(m, t, std::make_pair(s, ++value));
    }
    is_same(m, t);
    typedef std::vector<reference_type::value_type> values_t;
    values_t mv(m.begin(), m.end());
    values_t pv(t.begin(), t.end());
    BOOST_CHECK(std::is_permutation(mv.begin(), mv.end(), pv.begin()));
  }
}

template <template<typename...> class T>
void test_insert_remove() {
  test_scenarii<T>( {"tést", "test", "etre", "testera", "", "tester", "-test", "tertre", "atre", "etait", "était"});
  test_scenarii<T>( { "test", "testar", "testut", "-test" });
  test_scenarii<T>( { "test", "testar", "testut", "-testar", "-testut" });
  test_scenarii<T>( { "test", "te", "tesab", "teracl", "tero", "terobl" });

  T<std::string, int> t = { { "test", 8}, { "other", 5 } };
  T<std::string, int> const& tc = t;
  BOOST_CHECK_EQUAL(t["test"], 8);
  tc.root().find("test").value()->second = 2;
  BOOST_CHECK_EQUAL(t["test"], 2);
}

BOOST_AUTO_TEST_CASE(trie) {
  test_insert_remove<textip::trie>();
}

BOOST_AUTO_TEST_CASE(datrie) {
  test_insert_remove<textip::datrie>();
}

BOOST_AUTO_TEST_CASE(ptrie) {
  test_insert_remove<textip::ptrie>();
}
