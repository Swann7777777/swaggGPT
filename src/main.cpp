#include <iostream>
#include <string>
#include "trie.hpp"
#include "vocabulary.hpp"
#include "dataset.hpp"
#include "embeddings.hpp"
#include "file.hpp"
#include "softmax.hpp"


int main() {

    const std::string vocabularyFilePath = "../resources/vocabulary.txt";
    const std::string datasetFilePath = "../resources/training/wikitext-103/wiki.test.tokens";
    const std::string embeddingsFilePath = "../resources/embeddings.bin";

    // The maximum amount of tokens the code will load at once
    const int batchSize = 1000;
    const int embeddingDimensions = 100;
    const float learningRate = 0.001f;
    const int contextWindowSize = 5;

    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    const int nodeCount = vocabulary.tokens.size() - 1;

    trieClass trie;
    trie.generate(vocabulary.tokens);
    
    datasetClass dataset(datasetFilePath, batchSize);
    dataset.buildFrequencyMap(trie, vocabulary.tokens.size());
    
    embeddingsClass embeddings(embeddingDimensions);
    embeddings.generateRandom(vocabulary.tokens.size(), nodeCount);
    
    softmaxClass hSoftmax;
    hSoftmax.buildTree(dataset.tokenFrequencies, nodeCount);
    hSoftmax.buildPaths();

    
    fileClass file;
    //file.save(embeddings, embeddingsFilePath);
    //file.load(embeddingsFilePath, vocabulary.tokens.size(), embeddings, embeddingDimensions, nodeCount);

    
    // Train until end of corpus file
    while(dataset.loadBatch(trie)) {

        std::vector<std::pair<std::vector<float>, int>> inputGradient(vocabulary.tokens.size(), {std::vector<float>(embeddingDimensions, 0.0f), 0});

        std::vector<std::pair<std::vector<float>, int>> outputGradient(embeddings.outputLayer.size(), {std::vector<float>(embeddingDimensions, 0.0f), 0});

        // Holds the sum of the loss over the batch to average later
        float lossAccumulator = 0.0f;

        // Keeps track of the number of gradients computations to divide the loss accumulator when averaging
        int iterations = 0;

        // Iterate over the batch tokens
        for (int i = 0; i < dataset.tokens.size(); i++) {
            
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

                embeddings.backwardPass(targetIndex, contextIndex, hSoftmax, inputGradient[targetIndex].first, outputGradient);

                inputGradient[targetIndex].second++;

                lossAccumulator += embeddings.loss(targetIndex, contextIndex, hSoftmax);

                iterations++;
            }

        }

        for (int i = 0; i < vocabulary.tokens.size(); i++) {

            for (int j = 0; j < embeddingDimensions; j++) {

                embeddings.inputLayer[i][j] -= inputGradient[i].first[j] * learningRate / inputGradient[i].second;
            }
        }

        for (int i = 0; i < embeddings.outputLayer.size(); i++) {

            for (int j = 0; j < embeddingDimensions; j++) {

                embeddings.outputLayer[i][j] -= outputGradient[i].first[j] * learningRate / inputGradient[i].second;
            }
        }



        std::cout << lossAccumulator / static_cast<float>(iterations) << "\n";
    }

    // Save the embeddings to the binary file
    // file.save(embeddings, embeddingsFilePath);

    return 0;
}