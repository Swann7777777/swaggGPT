#include "softmax.hpp"
#include "embeddings.hpp"

float softmaxClass::softmax(const int &targetIndex, const int &contextIndex, embeddingsClass &embeddings) {

    float product = 1;

    for (int i = 0; i < paths[contextIndex].directions.size(); i++) {

        float dotProduct = std::inner_product(embeddings.inputLayer[targetIndex].begin(),
            embeddings.inputLayer[targetIndex].end(),
            embeddings.outputLayer[paths[contextIndex].path[i]->embeddingIndex].begin(), 0.0f);

        float condition = paths[contextIndex].directions[i] == right ? 1 : -1;

        float sigmoid = embeddings.sigmoid(condition * dotProduct);

        product *= sigmoid;
    }

    return product;
}