#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include "trie.hpp"

class datasetClass {
    public:

    int maxTokens = 0;

    std::vector<int> tokens;

    std::ifstream file;

    datasetClass(std::string &filePath, int &maxTokens) {

        this->maxTokens = maxTokens;

        // Open the dataset file
        file.open(filePath);

        // Verify that the file is open
        if (!file.is_open()) {
            std::cerr << "The dataset file could not be opened at path " << filePath << "\n";
            exit(1);
        }
    }

    void parse(trieClass &trie) {

        std::string line = "";

        
        // Iterate over dataset lines
        while (getline(file, line)) {
            
            // The current character should be ignored if this variable is true
            bool ignore = false;

            // The current word
            std::string accumulator = "";

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
                    if (!accumulator.empty()) {

                        // Tokenize the current word
                        std::vector<int> tokenizedWord = trie.tokenize(accumulator);

                        // Add the tokenized word to the tokens list
                        tokens.insert(tokens.end(), tokenizedWord.begin(), tokenizedWord.end());

                        // Stop parsing once the expected size is reached
                        if (tokens.size() >= maxTokens) {
                            return;
                        }

                        // Clear the character accumulator
                        accumulator = "";
                    }

                    continue;
                }

                // Add the current character to the character accumulator
                else if (!ignore) {

                    // Check if character is alpha before pushing it
                    if (std::isalpha(c)) {
                        accumulator += static_cast<char>(std::tolower(c));
                    }
                }
            }

            // The character accumulator isn't empty
            if (!accumulator.empty()) {

                std::vector<int> tokenizedWord = trie.tokenize(accumulator);

                tokens.insert(tokens.end(), tokenizedWord.begin(), tokenizedWord.end());
            }
        }

        return;
    }
};