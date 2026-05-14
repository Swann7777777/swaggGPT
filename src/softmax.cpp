#include "softmax.hpp"
#include "embeddings.hpp"

float softmaxClass::softmax(const int &targetIndex, const int &contextIndex, embeddingsClass &embeddings) {

    // Holds the product of the sigmoids of the dot products of the embeddings of each node on the path to the token
    float product = 1;

    // Iterate through the path in the Huffman tree to the token
    for (int i = 0; i < static_cast<int>(paths[contextIndex].directions.size()); i++) {

        // Compute the dot product of the target embedding and the current node's embedding
        float dotProduct = std::inner_product(embeddings.inputLayer[targetIndex].begin(),
            embeddings.inputLayer[targetIndex].end(),
            embeddings.outputLayer[paths[contextIndex].path[i]->embeddingIndex].begin(), 0.0f);

        // Is 1 if the current direction is an arbitrary chosen direction (right here) else -1
        float condition = paths[contextIndex].directions[i] == right ? 1 : -1;

        // Compute the sigmoid of the dot product multiplied by the condition variable
        float sigmoid = embeddings.sigmoid(condition * dotProduct);

        // Add the sigmoid to the product
        product *= sigmoid;
    }

    return product;
}