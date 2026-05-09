#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <random>
#include <iostream>
#include "softmax.hpp"

class embeddingsClass {
    public:

    int dimensions;

    // For now, these are vectors of vectors but I might change them to flat vectors for better speeds
    std::vector<std::vector<float>> inputLayer;
    std::vector<std::vector<float>> outputLayer;

    embeddingsClass(const int &dimensions) {

        this->dimensions = dimensions;
    }

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

    // Returns de probability of two tokens appearing in the context window
    float softmax(const std::vector<float>* targetEmbedding, const std::vector<float>* contextEmbedding) {

        // Holds the softmax fraction denominator
        float sum = 0.0f;

        // Compute the sum of the dot products of the target word and all the words in the vocabulary (not efficient -> should implement hierarchical softmax)
        for (const auto &w : outputLayer) {

            float dotProduct = std::inner_product(targetEmbedding->begin(), targetEmbedding->end(), w.begin(), 0.0f);
            sum += std::exp(dotProduct);
        }

        // Numerator of the fraction
        float tcDotProduct = std::inner_product(targetEmbedding->begin(), targetEmbedding->end(), contextEmbedding->begin(), 0.0f);

        return std::exp(tcDotProduct) / sum;
    }

    float loss(const int &targetIndex, const int &contextIndex, softmaxClass &hSoftmax) {

        float loss = - std::log(hSoftmax.softmax(targetIndex, contextIndex, *this));

        return std::isnan(loss) || std::isinf(loss) ? 0 : loss;
    }

    // Computes the input and output gradients
    void backwardPass(const int &targetIndex, const int &contextIndex, softmaxClass &hSoftmax, std::vector<float> &inputGradient, std::vector<std::pair<std::vector<float>, int>> &outputGradient) {

        softmaxClass::pathStruct* contextPath = &hSoftmax.paths[contextIndex];

        for (int i = 0; i < contextPath->directions.size(); i++) {

            float condition = contextPath->directions[i] == softmaxClass::right ? 1 : -1;

            float dotProduct = std::inner_product(outputLayer[contextPath->path[i]->embeddingIndex].begin(),
                outputLayer[contextPath->path[i]->embeddingIndex].end(),
                inputLayer[targetIndex].begin(), 0.0f);

            float lossTerm = - (condition - sigmoid(dotProduct));

            for (int j = 0; j < dimensions; j++) {

                outputGradient[contextPath->path[i]->embeddingIndex].first[j] += inputLayer[targetIndex][j] * lossTerm;
                inputGradient[j] += outputLayer[contextPath->path[i]->embeddingIndex][j] * lossTerm;
            }

            outputGradient[contextPath->path[i]->embeddingIndex].second++;
        }
    }
};