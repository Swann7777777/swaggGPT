#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "embeddings.hpp"
#include "vocabulary.hpp"

class fileClass {
    public:

    // Load the binary embedding file into memory
    void load(const std::string &filePath, const int &vocabularySize, embeddingsClass &embeddings, const int &dimensions, const int &nodeCount) {

        
        std::ifstream file(filePath, std::ios::in);
        
        // Check if file was successfully opened
        if (!file.is_open()) {
            std::cerr << "Could not open embedding file at path " << filePath << " during loading\n";
            exit(1);
        }
        
        // Check if the file is the right size
        if (std::filesystem::file_size(filePath) != (dimensions * sizeof(float)) * (vocabularySize + nodeCount)) {
            std::cerr << "Incorrect embeddings file size\n";
            file.close();
            exit(1); 
        }

        // Resize vectors to prevent overflow
        embeddings.inputLayer.resize(vocabularySize);
        embeddings.outputLayer.resize(nodeCount);

        // Read the input layer embeddings
        for (auto &i : embeddings.inputLayer) {

            i.resize(dimensions);
            file.read(reinterpret_cast<char*>(i.data()), dimensions * sizeof(float));
        }

        // Read the output layer embeddings
        for (auto &i : embeddings.outputLayer) {

            i.resize(dimensions);
            file.read(reinterpret_cast<char*>(i.data()), dimensions * sizeof(float));
        }

        file.close();
    }

    // Saves the embeddings to a binary file
    void save (embeddingsClass &embeddings, const std::string &filePath) {

        std::ofstream file(filePath, std::ios::binary);

        // Check if file was successfully opened
        if (!file.is_open()) {
            std::cerr << "Could not open embedding file at path " << filePath << " during saving\n";
            exit(1);
        }

        // Write the input layer embeddings
        for (auto &i : embeddings.inputLayer) {
            file.write(reinterpret_cast<char*>(i.data()), sizeof(float) * i.size());
        }

        // Write the output layer embeddings
        for (auto &i : embeddings.outputLayer) {
            file.write(reinterpret_cast<char*>(i.data()), sizeof(float) * i.size());
        }

        file.close();
    }
};