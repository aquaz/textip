add_executable(test_trie test_trie.cpp)
target_link_libraries(test_trie ${Boost_LIBRARIES})
add_test(trie test_trie)

set(CELERO_PATH ../../celero)
include_directories(${CELERO_PATH}/include)
find_library(CELERO_LIB celero ${CELERO_PATH}/build)
add_executable(benchmark_trie benckmark_trie.cpp)
target_link_libraries(benchmark_trie ${CELERO_LIB})

add_executable(profile profile.cpp)