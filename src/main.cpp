#include <iostream>
#include <string>
#include "trie.hpp"
#include "vocabulary.hpp"
#include "dataset.hpp"
#include "embeddings.hpp"
#include "file.hpp"


int main() {

    const std::string vocabularyFilePath = "../resources/vocabulary.txt";
    const std::string datasetFilePath = "../resources/training/wikitext-103/wiki.test.tokens";
    const std::string embeddingsFilePath = "../resources/embeddings.bin";

    // The maximum amount of tokens the code will load at once
    int maxTokens = 1000;
    int embeddingDimensions = 100;

    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    trieClass trie;
    trie.generate(vocabulary.tokens);
    
    embeddingsClass embeddings(embeddingDimensions);
    //embeddings.generateRandom(vocabulary.tokens.size());
    
    fileClass file;
    file.load(embeddingsFilePath, vocabulary.tokens.size(), embeddings.embeddings, embeddingDimensions);
    //file.save(embeddings.embeddings, embeddingsFilePath);
    
    datasetClass dataset(datasetFilePath, maxTokens);
    dataset.parse(trie);
    
    std::cout << embeddings.loss(dataset.tokens[0], dataset.tokens[1]) << "\n";

    return 0;
}
