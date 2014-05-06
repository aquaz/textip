#include <celero/Celero.h>
#include "../trie/trie.hpp"
#include <unordered_map>
#include <map>
#include <fstream>
#include <algorithm>
#include <iostream>

CELERO_MAIN

struct FromFileBench : public celero::TestFixture {
  FromFileBench() {
    std::ifstream ifs ( "../words.txt" );
    std::string s;

    while ( ifs >> s ) {
      values_.push_back ( {s, 1} );
    }
  }

  template <template <typename,typename, typename ...> class T>
  void fill() {
    T<std::string, int> t ( values_.begin(), values_.end() );
  }
  std::vector<std::pair<std::string, int>> values_;
};

template <template <typename,typename, typename ...> class T>
struct KeyAccessBench : public FromFileBench {
  KeyAccessBench() {
    std::random_shuffle ( values_.begin(), values_.end() );
    std::size_t tier = values_.size() / 3;
    t_.insert ( values_.begin(), values_.begin() + 2 * tier );
    std::for_each ( values_.begin() + tier, values_.end(), [this] ( std::pair<std::string, int> const& x ) {
      lookup_.push_back ( x.first );
    } );
  }
  void read() {
    std::accumulate ( lookup_.begin(), lookup_.end(), 0, [this] ( int a, std::string const& b ) {
      auto it = t_.find ( b );
      return a + ( it == t_.end() ? 0 : 1 );
    } );
  }
  T<std::string, int> t_;
  std::vector<std::string> lookup_;
};

BASELINE_F ( FillBenchmark, StdHash, FromFileBench, 0, 10 ) {
  fill<std::unordered_map>();
}

BENCHMARK_F ( FillBenchmark, StdMap, FromFileBench, 0, 10 ) {
  fill<std::map>();
}

BENCHMARK_F ( FillBenchmark, trie, FromFileBench, 0, 10 ) {
  fill<textip::trie>();
}

BASELINE_F ( KeyAccessBenchmark, StdHash, KeyAccessBench<std::unordered_map>, 0, 10 ) {
  read();
}

BENCHMARK_F ( KeyAccessBenchmark, StdMap, KeyAccessBench<std::map>, 0, 10 ) {
  read();
}

BENCHMARK_F ( KeyAccessBenchmark, trie, KeyAccessBench<textip::trie>, 0, 10 ) {
  read();
}
