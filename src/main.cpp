#include <iostream>
#include <string>
#include "trie.hpp"
#include "vocabulary.hpp"
#include "dataset.hpp"

int main() {

    std::string vocabularyFilePath = "../resources/vocabulary.txt";
    std::string datasetFilePath = "../resources/training/wikitext-103/wiki.test.tokens";

    // The maximum amount of tokens the code will load at once
    int maxTokens = 1000;

    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    trieClass trie;
    trie.generate(vocabulary.tokens);

    datasetClass dataset(datasetFilePath, maxTokens);

    dataset.parse(trie);

    for (const auto &i : dataset.tokens) {
        std::cout << vocabulary.tokens[i] << " ";
    }

    std::cout << std::endl;

    return 0;
}
