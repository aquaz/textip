#ifndef TEXTIP_TEST_MATERIAL_H
#define TEXTIP_TEST_MATERIAL_H

#include <ctime>
#include <cstdlib>

#include <algorithm>
#include <fstream>
#include <vector>

class Samples {
public:

  Samples& reset() { i = 0; return *this; }
  auto& get() {
    if (++i >= values.size())
      i = 0;
    return values[i];
  }
  static Samples& instance() {
    static Samples ret;
    return ret;
  }
  std::vector<std::pair<std::string, int>> values;
private:
  Samples() {
    std::ifstream ifs("../../../words.txt");
    if (!ifs) throw std::runtime_error("No file");
    std::string s;

    while (ifs >> s) {
      values.push_back( {s, 1});
    }
    std::time_t t = std::time(NULL);
    std::srand(t);
    std::random_shuffle(values.begin(), values.end());
  }
  std::size_t i = 0;
};


Samples& first_ = Samples::instance();

#endif /* !TEXTIP_TEST_MATERIAL_H */
