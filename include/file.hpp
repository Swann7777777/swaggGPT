#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "embeddings.hpp"
#include "vocabulary.hpp"

class fileClass {
    public:

    void load(std::string &filePath, vocabularyClass &vocabulary, embeddingsClass &embeddings) {

        std::ifstream file(filePath, std::ios::binary);

        embeddings.embeddings.resize(vocabulary.tokens.size());

        for (auto &i : embeddings.embeddings) {
            file.read(reinterpret_cast<char*>(i.data()), embeddings.dimensions * sizeof(float));
        }

        file.close();
    }

    void save(std::vector<std::vector<float>> &embeddings, std::string &filePath) {

        std::ofstream file(filePath, std::ios::binary);

        for (auto &i : embeddings) {
            file.write(reinterpret_cast<char*>(i.data()), sizeof(float) * i.size());
        }

        file.close();
    }
};