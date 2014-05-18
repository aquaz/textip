#include "../trie/trie.hpp"

#include "test_material.hpp"

template <template <typename ...Args> class T>
void profile() {
  T<std::string, int> t;
  for (auto& p : Samples::instance().values) {
    t.insert(p);
  }
}

int main() {
  profile<textip::datrie>();
}
