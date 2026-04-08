#include <fstream>
#include <string>
#include "dataset.hpp"
#include "vocabulary.hpp"
#include "trie.hpp"

int main() {

    std::string datasetFilePath("../resources/wikitext-103/wiki.test.tokens");
    std::string vocabularyFilePath("../resources/vocabulary.txt");

    // Load the vocabulary
    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    // Create the trie data structure
    trieClass trie;
    trie.generate(vocabulary.tokens);

    // Load the dataset
    datasetClass dataset;
    dataset.parse(datasetFilePath);

    return 0;
}