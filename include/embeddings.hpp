#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <random>
#include <iostream>

class embeddingsClass {
    public:

    int dimensions;

    // For now, these are vectors of vectors but I might change them to flat vectors for better speeds
    std::vector<std::vector<float>> inputLayer;
    std::vector<std::vector<float>> outputLayer;

    embeddingsClass(const int &dimensions) {

        this->dimensions = dimensions;
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

    // Returns the loss
    inline float loss(const float &softmax) {

        float loss = - std::log(softmax);

        // Check for floating point rounding error caused by std::log(0)
        return std::isnan(loss) || std::isinf(loss) ? 0 : loss;
    }

    // Returns the gradient
    std::vector<float> backwardPass(const float &softmax, const std::vector<float>* embedding) {

        // This variable holds de derivative of the loss function regarding the dot product of the embeddings
        float dLdzi = softmax - 1.0f;

        // The gradient is a weighted embedding
        std::vector<float> gradient = *embedding;
        
        // Multiply each element of the vector with the computed derivative
        for (int i = 0; i < dimensions; i++) {

            gradient[i] *= dLdzi;
        }

        return gradient;
    }
};