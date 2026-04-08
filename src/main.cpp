#include <fstream>
#include "dataset.hpp"
#include "vocabulary.hpp"
#include "trie.hpp"

int main() {

    std::ifstream datasetFile("../resources/wikitext-103/wiki.test.tokens");
    std::ifstream vocabularyFile("../resources/vocabulary.txt");

    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFile);

    trieClass trie(vocabulary.tokens);

    datasetClass dataset;
    dataset.parse(datasetFile);

    return 0;
}