#include <fstream>
#include <string>
#include "dataset.hpp"
#include "vocabulary.hpp"
#include "trie.hpp"

int main() {

    std::string datasetFilePath("../resources/training/testTokens.txt");
    // std::string datasetFilePath("../resources/training/wikitext-103/wiki.test.tokens");
    std::string vocabularyFilePath("../resources/vocabulary.txt");

    // Load the vocabulary
    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    // Create the trie data structure
    trieClass trie;
    trie.generate(vocabulary.tokens);

    // Load the dataset
    datasetClass dataset;
    dataset.parse(datasetFilePath, trie);

    for (const auto &i : dataset.tmpTokenizedWords) {
        for (const auto &j : i) {
            std::cout << vocabulary.tokens[j] << " ";
        }
        std::cout << "\n";
    }

    return 0;
}