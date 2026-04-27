#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <random>
#include <iostream>

class embeddingsClass {
    public:

    int dimensions;

    std::vector<std::vector<float>> embeddings;

    embeddingsClass(const int &dimensions) {

        this->dimensions = dimensions;
    }

    // Randomly initialize the weights with Glorot initialization 
    void generateRandom(const int &vocabularySize) {

        embeddings.resize(vocabularySize);

        float interval = std::sqrt(6) / std::sqrt(2 * dimensions);

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_real_distribution<float> dist(-interval, interval);

        for (auto &i : embeddings) {

            i.resize(dimensions);

            for (auto &j : i) {

                j = dist(rng);
            }
        }
    }

    // Computes de probability of two tokens appearing in the context window
    float softmax(const std::vector<float> &targetToken, const std::vector<float> &contextToken) {

        float sum = 0.0f;

        for (const auto &w : embeddings) {

            float dotProduct = std::inner_product(targetToken.begin(), targetToken.end(), w.begin(), 0.0f);

            sum += std::exp(dotProduct);
        }

        float tcDotProduct = std::inner_product(targetToken.begin(), targetToken.end(), contextToken.begin(), 0.0f);

        return std::exp(tcDotProduct) / sum;
    }

    // Returns the loss
    float loss(const int &targetTokenIndex, const int &contextTokenIndex) {

        std::vector<float> targetToken = embeddings[targetTokenIndex];
        std::vector<float> contextToken = embeddings[contextTokenIndex];

        return - std::log(softmax(targetToken, contextToken));
    }

    // Returns the gradient
    std::vector<float> backwardPass(const float &softmax, const int &embeddingIndex) {

        float dLdzi = softmax - 1.0f;

        std::vector<float> gradient = embeddings[embeddingIndex];
        
        for (int i = 0; i < dimensions; i++) {

            gradient[i] *= dLdzi;
        }

        return gradient;
    }
};