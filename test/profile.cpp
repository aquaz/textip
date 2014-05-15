#include "../trie/trie.hpp"

#include "test_material.hpp"

template <template <typename ...Args> class T>
void profile() {
  T<std::string, int> t;
  for (auto& v : Samples::instance().values)
    t.insert(v);
}

int main() {
  profile<textip::trie>();
}
