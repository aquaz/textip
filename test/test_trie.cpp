#define BOOST_TEST_MODULE trie test
#include <boost/test/unit_test.hpp>

#include "../trie/trie.hpp"
#include <unordered_map>

BOOST_AUTO_TEST_CASE ( basic_operations ) {
  std::unordered_map<std::string, int> m = {
    {"test", 8},
    {"etre", 4},
    {"testera", 10},
    {"tester", 20},
    {"tertre", 40},
    {"tertrer", 48},
  };
  textip::Trie<std::string,int> t ( m.begin(), m.end() );
  for ( auto& p : m ) {
    BOOST_CHECK_EQUAL ( t[p.first], p.second );
  }
  for ( auto& p : t ) {
    BOOST_CHECK ( m.find ( p.first ) != m.end() );
  }
  std::string s = "lvalue";
  t[s] = 87;
  BOOST_CHECK ( t[s] == 87 );
  BOOST_CHECK ( t.find ( s )->second == 87 );
}
