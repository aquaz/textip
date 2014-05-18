#include "../trie/trie.hpp"

#include "test_material.hpp"

template <template <typename ...Args> class T>
void profile() {
  T<std::string, int> t;
  for (int i = 0; i < 10; ++i)
    t.insert(Samples::instance().get());
}

int main() {
  profile<textip::datrie>();
}
