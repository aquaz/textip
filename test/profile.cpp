#include "../trie/trie.hpp"

#include <unordered_map>
#include "test_material.hpp"

template <template <typename ...Args> class T>
void profile() {
  auto& v = Samples::instance().values;
  T<std::string, int> t(v.begin(), v.end());
}

int main() {
  profile<textip::datrie>();
//   profile<std::unordered_map>();
//   profile<textip::trie>();
}
