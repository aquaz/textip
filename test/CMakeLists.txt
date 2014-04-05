add_executable(test_trie test_trie.cpp)
target_link_libraries(test_trie ${Boost_LIBRARIES})
add_test(trie test_trie)