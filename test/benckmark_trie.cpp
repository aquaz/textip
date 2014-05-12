#include "../trie/trie.hpp"

#include <celero/Celero.h>
#include <unordered_map>
#include <map>
#include <fstream>
#include <algorithm>

#include <boost/preprocessor/seq/for_each.hpp>

CELERO_MAIN

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
    std::ifstream ifs("../words.txt");
    std::string s;

    while (ifs >> s) {
      values.push_back( {s, 1});
    }
    srand(time(NULL));
    std::random_shuffle(values.begin(), values.end());
  }
  std::size_t i = 0;
};

Samples& first_ = Samples::instance();

template <template <typename, typename, typename ...> class T>
struct Fill : public celero::TestFixture {
  void execute() {
    celero::DoNotOptimizeAway(t[samples.get().first] = 1);
  }
  T<std::string, int> t;
  Samples samples = Samples::instance().reset();
};

template <template <typename, typename, typename ...> class T>
struct KeyAccess : public celero::TestFixture {
  KeyAccess() {
    auto& values = samples.values;
    std::size_t third = values.size() / 3;
    t_.insert(values.begin(), values.begin() + 2 * third);
  }
  void execute() {
    t_.find(samples.get().first);
  }
  T<std::string, int> t_;
  Samples samples = Samples::instance().reset();
};

using namespace std;
using namespace textip;

#define BASELINE_TYPE unordered_map
#define TYPES (map)(trie)(trie_p)

const std::size_t nb_sample = 50;
const std::size_t sample_size = 1000000;

#define BENCH_TYPE_BASELINE(Test) \
  BASELINE_F(Test, BASELINE_TYPE, Test<BASELINE_TYPE>, nb_sample, sample_size) { \
    execute(); \
  }

#define BENCH_TYPE(r, data, elem) \
  BENCHMARK_F(data, elem, data<elem>, nb_sample, sample_size) { \
    execute(); \
  }

BENCH_TYPE_BASELINE(Fill)

BOOST_PP_SEQ_FOR_EACH(BENCH_TYPE, Fill, TYPES)

BENCH_TYPE_BASELINE(KeyAccess)

BOOST_PP_SEQ_FOR_EACH(BENCH_TYPE, KeyAccess, TYPES)
