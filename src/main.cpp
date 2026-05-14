#include <iostream>
#include <string>
#include <csignal>
#include <atomic>
#include <cmath>
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
    const float learningRate = 0.0025f;
    const int contextWindowSize = 5;

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


    // int token = 388;
    // int a = 132;
    // int b = 2111;
    // std::pair<int, float> bestMatch = {-1, -1.0f};

    // std::vector<float> embedding = embeddings.inputLayer[token];

    // for (int i = 0; i < embeddingDimensions; i++) {

    //     embedding[i] = embedding[i] - embeddings.inputLayer[a][i] + embeddings.inputLayer[b][i];
    // }

    // float magnitudeA = 0.0f;

    // for (int i = 0; i < embeddingDimensions; i++) {

    //     magnitudeA += std::pow(embedding[i], 2);
    // }

    // magnitudeA = std::sqrt(magnitudeA);

    // for (int i = 0; i < static_cast<int>(vocabulary.tokens.size()); i++) {

    //     if (i == token || i == a || i == b) {
    //         continue;
    //     }

    //     float dotProduct = std::inner_product(embedding.begin(), embedding.end(), embeddings.inputLayer[i].begin(), 0.0f);

    //     float magnitudeB = 0.0f;

    //     for (int j = 0; j < embeddingDimensions; j++) {

    //         magnitudeB += std::pow(embeddings.inputLayer[i][j], 2);
    //     }

    //     magnitudeB = std::sqrt(magnitudeB);

    //     float cosineSimilarity = dotProduct / (magnitudeA * magnitudeB);

    //     if (cosineSimilarity > bestMatch.second) {
    //         bestMatch = {i, cosineSimilarity};
    //     }
    // }

    // std::cout << vocabulary.tokens[bestMatch.first] << "\n";

    // return 0;
    

    // Keeps track of the number of batches processed
    int batchIterations = 0;

    // Initialize a listener for SIGINT events ( when the user presses CTRL + C in the running console )
    signal(SIGINT, signalHandler);

    // Train until end of corpus file
    while (dataset.loadBatch(trie) && train) {

        // Holds the sum of the loss over the batch to average later
        float lossAccumulator = 0.0f;

        // Keeps track of the number of gradients computations to divide the loss accumulator when averaging
        int iterations = 0;

        // Iterate over the batch tokens
        for (int i = 0; i < static_cast<int>(dataset.tokens.size()); i++) {
            
            // Store the index of the target token for easier access and to prevent confusion
            int targetIndex = dataset.tokens[i];

            // Calculate the context window limits
            int jInit = std::max(0, i - ((contextWindowSize - 1) / 2));
            int jMax = std::min(static_cast<int>(dataset.tokens.size()), i + ((contextWindowSize - 1) / 2) + 1);

            // Iterate through the context window
            for (int j = jInit; j < jMax; j++) {

                // Skips computing the gradient of the target token with itself
                if (j == i) {
                    continue;
                }                

                // Store the index of the context token for easier access and to prevent confusion
                int contextIndex = dataset.tokens[j];

                // Run the backpropagation algorithm to update the embeddings of the model
                embeddings.backwardPass(targetIndex, contextIndex, hSoftmax);

                // Add the current loss to the loss accumulator
                lossAccumulator += embeddings.loss(targetIndex, contextIndex, hSoftmax); // SOFTMAX RESULT IS COMPUTED TWICE BECAUSE IT WAS ALREADY COMPUTED IN backwardPass()

                // Increment the iterations to divide the loss accumulator
                iterations++;
            }

        }

        std::cout << "(" << batchIterations << "," << lossAccumulator / static_cast<float>(iterations) << "),";
        batchIterations++;
    }

    std::cout << "Saving embeddings...\n";

    // Save the embeddings to the binary file
    file.save(embeddings, embeddingsFilePath);

    return 0;
}