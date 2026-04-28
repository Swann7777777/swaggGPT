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
    int batchSize = 1000;
    int embeddingDimensions = 100;
    float learningRate = 0.001f;
    int contextWindowSize = 5;

    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    trieClass trie;
    trie.generate(vocabulary.tokens);
    
    embeddingsClass embeddings(embeddingDimensions);

    fileClass file;

    embeddings.generateRandom(vocabulary.tokens.size());
    file.save(embeddings, embeddingsFilePath);
    
    file.load(embeddingsFilePath, vocabulary.tokens.size(), embeddings, embeddingDimensions);
    
    datasetClass dataset(datasetFilePath, batchSize);


    std::vector<float> zeroVector(embeddingDimensions, 0.0f);

    // Train until end of corpus file
    while(dataset.parse(trie)) {

        std::vector<std::pair<std::vector<float>, int>> inputGradientAccumulator(vocabulary.tokens.size(), {zeroVector, 0});
        std::vector<std::pair<std::vector<float>, int>> outputGradientAccumulator(vocabulary.tokens.size(), {zeroVector, 0});

        float lossAccumulator = 0.0f;

        int iterations = 0;


        // Iterate over the batch tokens
        for (int i = 0; i < dataset.tokens.size(); i++) {
            
            std::vector<float>* targetEmbedding = &embeddings.inputLayer[dataset.tokens[i]];

            // Calculate the context window limits
            int jInit = std::max(0, i - ((contextWindowSize - 1) / 2));
            int jMax = std::min(static_cast<int>(dataset.tokens.size()), i + ((contextWindowSize - 1) / 2) + 1);

            // Iterate through the context window
            for (int j = jInit; j < jMax; j++) {

                if (j == i) {
                    continue;
                }

                std::vector<float> *contextEmbedding = &embeddings.outputLayer[dataset.tokens[j]];

                float softmax = embeddings.softmax(targetEmbedding, contextEmbedding);

                std::vector<float> inputGradient = embeddings.backwardPass(softmax, contextEmbedding);
                
                std::vector<float> outputGradient = embeddings.backwardPass(softmax, targetEmbedding);

                for (int k = 0; k < embeddingDimensions; k++) {

                    inputGradientAccumulator[dataset.tokens[i]].first[k] += inputGradient[k];
                    outputGradientAccumulator[dataset.tokens[j]].first[k] += outputGradient[k];
                }

                inputGradientAccumulator[dataset.tokens[i]].second++;
                outputGradientAccumulator[dataset.tokens[j]].second++;

                lossAccumulator += embeddings.loss(softmax);
                iterations++;
            }

        }

        for (int j = 0; j < vocabulary.tokens.size(); j++) {

            if (inputGradientAccumulator[j].second > 0) {

                for (int k = 0; k < embeddingDimensions; k++) {

                    embeddings.inputLayer[j][k] -= inputGradientAccumulator[j].first[k] * learningRate / inputGradientAccumulator[j].second;
                }
            }

            if (outputGradientAccumulator[j].second > 0) {

                for (int k = 0 ; k < embeddingDimensions; k++) {

                    embeddings.outputLayer[j][k] -= outputGradientAccumulator[j].first[k] * learningRate / inputGradientAccumulator[j].second;
                }
            }
        }

        std::cout << lossAccumulator / static_cast<float>(iterations) << "\n";
    }

    
    
    file.save(embeddings, embeddingsFilePath);

    return 0;
}