#include <fstream>
#include <string>
#include "dataset.hpp"
#include "vocabulary.hpp"

int main() {

    std::string datasetFilePath("../resources/wikitext-103/wiki.test.tokens");
    std::string vocabularyFilePath("../resources/vocabulary.txt");

    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    datasetClass dataset;
    dataset.parse(datasetFilePath);

    return 0;
}