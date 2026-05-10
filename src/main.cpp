#include <iostream>
#include <string>
#include <csignal>
#include <atomic>
#include "trie.hpp"
#include "vocabulary.hpp"
#include "dataset.hpp"
#include "embeddings.hpp"
#include "file.hpp"
#include "softmax.hpp"


std::atomic<bool> train(true);

void signalHandler(int sig) {

    train = false;
}


int main() {

    const std::string vocabularyFilePath = "../resources/vocabulary.txt";
    const std::string datasetFilePath = "../resources/training/wikitext-103/wiki.train.tokens";
    const std::string embeddingsFilePath = "../resources/embeddings.bin";

    // The maximum amount of tokens the code will load at once
    const int batchSize = 10000;
    const int embeddingDimensions = 100;
    const float learningRate = 0.002f;
    const int contextWindowSize = 11;

    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    const int nodeCount = vocabulary.tokens.size() - 1;

    trieClass trie;
    trie.generate(vocabulary.tokens);
    
    datasetClass dataset(datasetFilePath, batchSize);
    dataset.buildFrequencyMap(trie, vocabulary.tokens.size());
    
    embeddingsClass embeddings(embeddingDimensions, learningRate);
    embeddings.generateRandom(vocabulary.tokens.size(), nodeCount);
    
    softmaxClass hSoftmax;
    hSoftmax.buildTree(dataset.tokenFrequencies, nodeCount);
    hSoftmax.buildPaths();
    
    fileClass file;
    // file.load(embeddingsFilePath, vocabulary.tokens.size(), embeddings, embeddingDimensions, nodeCount);


    // int token = 1742;
    // std::pair<int, float> bestMatch = {-1, 0.0f};

    // for (int i = 0; i < static_cast<int>(vocabulary.tokens.size()); i++) {

    //     if (i == token) {
    //         continue;
    //     }

    //     float dotProduct = std::inner_product(embeddings.inputLayer[i].begin(), embeddings.inputLayer[i].end(), embeddings.inputLayer[token].begin(), 0.0f);

    //     if (dotProduct > bestMatch.second) {
    //         bestMatch = {i, dotProduct};
    //     }
    // }

    // std::cout << vocabulary.tokens[bestMatch.first] << "\n";

    // return 0;
    

    signal(SIGINT, signalHandler);

    // Train until end of corpus file
    while (dataset.loadBatch(trie) && train) {

        // Holds the sum of the loss over the batch to average later
        float lossAccumulator = 0.0f;

        // Keeps track of the number of gradients computations to divide the loss accumulator when averaging
        int iterations = 0;

        // Iterate over the batch tokens
        for (int i = 0; i < static_cast<int>(dataset.tokens.size()); i++) {
            
            int targetIndex = dataset.tokens[i];

            // Calculate the context window limits
            int jInit = std::max(0, i - ((contextWindowSize - 1) / 2));
            int jMax = std::min(static_cast<int>(dataset.tokens.size()), i + ((contextWindowSize - 1) / 2) + 1);

            // Iterate through the context window
            for (int j = jInit; j < jMax; j++) {

                // Skips computing the gradient of the target word with itself
                if (j == i) {
                    continue;
                }                

                int contextIndex = dataset.tokens[j];

                embeddings.backwardPass(targetIndex, contextIndex, hSoftmax);

                lossAccumulator += embeddings.loss(targetIndex, contextIndex, hSoftmax);

                iterations++;
            }

        }

        std::cout << lossAccumulator / static_cast<float>(iterations) << "\n";
    }

    std::cout << "Saving embeddings...\n";

    // Save the embeddings to the binary file
    file.save(embeddings, embeddingsFilePath);

    return 0;
}