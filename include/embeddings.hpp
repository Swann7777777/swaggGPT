#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <random>
#include <iostream>
#include "softmax.hpp"

class embeddingsClass {
    public:

    int dimensions = 0;
    float learningRate = 0.0f;

    // For now, these are vectors of vectors but I might change them to flat vectors for better speeds
    std::vector<std::vector<float>> inputLayer;
    std::vector<std::vector<float>> outputLayer;

    embeddingsClass(const int &dimensions, const float &learningRate) {

        this->dimensions = dimensions;
        this->learningRate = learningRate;
    }

    // Computes the sigmoid of x
    static inline float sigmoid(const float &x) {

        return 1/(1 + std::exp(-x));
    }

    // Randomly initialize the weights with Glorot initialization 
    void generateRandom(const int &vocabularySize, const int &nodeCount) {

        // Make sure the vectors are of the right size
        inputLayer.resize(vocabularySize);
        outputLayer.resize(nodeCount);

        // Compute the interval according to the Glorot initialization
        float interval = std::sqrt(6) / std::sqrt(2 * dimensions);

        // Initialize the random number generator
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_real_distribution<float> dist(-interval, interval);

        // Randomize the embeddings of the input layer
        for (auto &i : inputLayer) {
            i.resize(dimensions);

            for (auto &j : i) {
                j = dist(rng);
            }
        }

        // Randomize the embedding of the output layer
        for (auto &i : outputLayer) {
            i.resize(dimensions);

            for (auto &j : i) {
                j = dist(rng);
            }
        }
    }

    // Returns the loss of the target token with the context token
    float loss(const int &targetIndex, const int &contextIndex, softmaxClass &hSoftmax) {

        // Computes the loss with negative log likelyhood
        float loss = - std::log(hSoftmax.softmax(targetIndex, contextIndex, *this));

        // REMOVED THE SAFEGUARD THAT PREVENTS THE LOSS FROM BEING NaN OR INF FOR TESTING, CONSIDER REMOVING IT LATER MAYBE
        return loss;
        // return std::isnan(loss) || std::isinf(loss) ? 0 : loss;
    }

    // Computes the input and output gradients
    void backwardPass(const int &targetIndex, const int &contextIndex, softmaxClass &hSoftmax) {

        // Initialize the input gradient vector which only updates the weights at the end of the function
        std::vector<float> inputGradient(dimensions, 0.0f);

        // Store a pointer to the path to the context token for easier access
        softmaxClass::pathStruct* contextPath = &hSoftmax.paths[contextIndex];

        // Iterate through the path to the leaf holding the context token in the Huffman tree
        for (int i = 0; i < static_cast<int>(contextPath->directions.size()); i++) {

            // Holds either a 1 if the current direction is the arbitrarily chosen direction (here right) else 0
            float condition = contextPath->directions[i] == softmaxClass::right ? 1.0f : 0.0f;

            // Compute the dot product of the target embedding with the current node's embedding
            float dotProduct = std::inner_product(outputLayer[contextPath->path[i]->embeddingIndex].begin(),
                outputLayer[contextPath->path[i]->embeddingIndex].end(),
                inputLayer[targetIndex].begin(), 0.0f);

            // Compute the loss term from the condition variable and the sigmoid of the dot product
            float lossTerm = - (condition - sigmoid(dotProduct));

            // Update the embeddings with the computed gradients
            for (int j = 0; j < dimensions; j++) {

                // Add the current gradient to the input gradient
                inputGradient[j] += outputLayer[contextPath->path[i]->embeddingIndex][j] * lossTerm;

                // Directly update the output layer
                outputLayer[contextPath->path[i]->embeddingIndex][j] -= inputLayer[targetIndex][j] * lossTerm * learningRate;
            }
        }

        // Update the input layer
        for (int i = 0; i < dimensions; i++) {

            inputLayer[targetIndex][i] -= inputGradient[i] * learningRate;
        }
    }
};