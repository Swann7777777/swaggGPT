#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include "trie.hpp"

class datasetClass {
    public:

    std::vector<std::vector<int>> tmpTokenizedWords;

    void parse(std::string &filePath, trieClass &trie) {

        // Open the dataset file
        std::ifstream file(filePath);

        // Verify that the file is open
        if (!file.is_open()) {
            std::cerr << "The dataset file could not be opened at path " << filePath << "\n";
            exit(1);
        }

        std::string line = "";

        
        // Iterate over dataset lines
        while (getline(file, line)) {
            
            // The current character should be ignored if this variable is true
            bool ignore = false;

            // The current word
            std::string word = "";

            // Iterate over line characters
            for (const auto &c : line) {
                
                if (c == '<') {
                    ignore = true;
                }

                else if (c == '>') {
                    ignore = false;
                }

                // These characters mark the end of a word
                else if (c == ' ' || c == '.' || c == '-' || c == '\'') {

                    // The character accumulator isn't empty
                    if (!word.empty()) {

                        // Tokenize the word
                        tmpTokenizedWords.push_back(trie.tokenize(word));

                        word = "";
                    }

                    continue;
                }

                // Add the current character to the character accumulator
                else if (!ignore) {

                    // Check if character is alpha before pushing it
                    if (std::isalpha(c)) {
                        word += static_cast<char>(std::tolower(c));
                    }
                }
            }

            // The character accumulator isn't empty
            if (!word.empty()) {

                // Tokenize the word
                tmpTokenizedWords.push_back(trie.tokenize(word));
            }
        }
    }
};