#include <string>
#include <gtest/gtest.h>

#include <rsl/trie>

TEST(Trie, Matches) {
  auto trie = rsl::Trie{{"apple", "banana", "cherry"}};

  EXPECT_TRUE(trie.matches("apple"));
  EXPECT_TRUE(trie.matches("banana"));
  EXPECT_TRUE(trie.matches("cherry"));

  EXPECT_FALSE(trie.matches("pear"));
  EXPECT_FALSE(trie.matches(""));
  EXPECT_FALSE(trie.matches("APPLE"));
}

TEST(Trie, Find) {
  auto trie = rsl::Trie{{"apple", "banana", "cherry"}};

  EXPECT_EQ(trie.find("apple"), 0);
  EXPECT_EQ(trie.find("banana"), 1);
  EXPECT_EQ(trie.find("cherry"), 2);

  EXPECT_EQ(trie.find("pear"), -1);
  EXPECT_EQ(trie.find(""), -1);
  EXPECT_EQ(trie.find("APPLE"), -1);
}