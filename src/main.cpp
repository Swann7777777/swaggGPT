#include <fstream>
#include <string>
#include "dataset.hpp"
#include "vocabulary.hpp"
#include "trie.hpp"
#include "pairs.hpp"

int main() {

    // std::string datasetFilePath("../resources/training/testTokens.txt");
    std::string datasetFilePath("../resources/training/wikitext-103/wiki.test.tokens");
    std::string vocabularyFilePath("../resources/vocabulary.txt");

    // Load the vocabulary
    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    // Create the trie data structure
    trieClass trie;
    trie.generate(vocabulary.tokens);

    // Initialize the pairs class
    pairsClass pairs;

    // Load the dataset
    datasetClass dataset;
    dataset.parse(datasetFilePath, trie, pairs);

    // Lookup the most frequent pair
    std::pair<int, int> mostFrequentPair;
    int bestFrequency = 0;

    for (const auto &[pair, frequency] : pairs.frequencies) {
        if (frequency > bestFrequency) {
            bestFrequency = frequency;
            mostFrequentPair = pair;
        }
    }

    std::cout << vocabulary.tokens[mostFrequentPair.first] << vocabulary.tokens[mostFrequentPair.second] << "\n";

    return 0;
}