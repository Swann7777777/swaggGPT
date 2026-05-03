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
    
    
    std::ifstream file;
    
    datasetClass(const std::string &filePath, const int &batchSize) {
        
        this->batchSize = batchSize;
        
        // Open the dataset file
        file.open(filePath);
        
        // Verify that the file is open
        if (!file.is_open()) {
            std::cerr << "The dataset file could not be opened at path " << filePath << "\n";
            exit(1);
        }
    }

    // Maps each token to its frequency in the dataset
    std::unordered_map<int, int> tokenFrequencies;

    // Builds the tokenFrequencies map
    void buildFrequencyMap(trieClass &trie, const int &vocabularySize) {

        for (int i = 0; i < vocabularySize; i++) {

            tokenFrequencies[i] = 0;
        }




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

                        std::vector<int> tokens = trie.tokenize(accumulator);

                        for (const auto &i : tokens) {
                            tokenFrequencies[i]++;
                        }

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

                std::vector<int> tokens = trie.tokenize(accumulator);

                for (const auto &i : tokens) {
                    tokenFrequencies[i]++;
                }
            }
        }
    }


    int batchSize = 0;
    std::vector<int> tokens;

    // Loads a batch of the dataset and tokenizes it
    bool loadBatch(trieClass &trie) {

        std::string line = "";

        
        // Iterate over dataset lines
        while (true) {

            if (getline(file, line)) {
                
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
                            if (tokens.size() >= batchSize) {
                                return true;
                            }

                            // Clear the character accumulator
                            accumulator = "";
                        }

                        continue;
                    }

                    // Add the current character to the character accumulator
                    else if (!ignore) {

                        // Check if character is alpha before pushing it to the accumulator
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

            // End of file, returns false
            else {
                return false;
            }
        }
    }
};