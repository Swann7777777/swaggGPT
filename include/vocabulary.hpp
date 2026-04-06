#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class vocabularyClass {
    public:
    
    std::vector<std::string> tokens;

    void load(std::ifstream &file) {
        
        std::string line;

        while (std::getline(file, line)) {
            tokens.push_back(line);
        }

        std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

        for (int i = 0; i < alphabet.size(); i++) {
            if (tokens[i] != std::string(1, alphabet[i])) {
                std::cout << "The entire alphabet must be present at the beginning of the vocabulary\n";
            }
        }
    }
};