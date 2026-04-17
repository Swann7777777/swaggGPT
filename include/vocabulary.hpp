#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

class vocabularyClass {
    public:
    
    std::vector<std::string> tokens;

    void load(std::string &filePath) {

        // Open the vocabulary file
        std::ifstream file(filePath);

        // Verify that the file is open
        if (!file.is_open()) {
            std::cerr << "The vocabulary file could not be opened at path " << filePath << "\n";
            exit(1);
        }
        
        std::string line;

        while (std::getline(file, line)) {
            tokens.push_back(line);
        }

        // Check if the vocabulary contains the alphabet in the first 26 tokens
        std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

        for (int i = 0; i < alphabet.size(); i++) {
            if (tokens[i] != std::string(1, alphabet[i])) {
                std::cerr << "The entire alphabet must be present at the beginning of the vocabulary\n";
                exit(1);
            }
        }
    }
};