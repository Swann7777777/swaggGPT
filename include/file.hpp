#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "embeddings.hpp"
#include "vocabulary.hpp"

class fileClass {
    public:

    void load(const std::string &filePath, const int &vocabularySize, std::vector<std::vector<float>> &embeddings, const int &dimensions) {

        std::ifstream file(filePath, std::ios::in);

        if (!file.is_open()) {
            std::cerr << "Could not open embedding file at path " << filePath << " during loading\n";
            exit(1);
        }

        embeddings.resize(vocabularySize);

        for (auto &i : embeddings) {
            i.resize(dimensions);
            file.read(reinterpret_cast<char*>(i.data()), dimensions * sizeof(float));
        }

        file.close();
    }

    void save (std::vector<std::vector<float>> &embeddings, const std::string &filePath) {

        std::ofstream file(filePath, std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Could not open embedding file at path " << filePath << " during saving\n";
            exit(1);
        }

        for (auto &i : embeddings) {
            file.write(reinterpret_cast<char*>(i.data()), sizeof(float) * i.size());
        }

        file.close();
    }
};