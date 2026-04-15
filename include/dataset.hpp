#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include "trie.hpp"
#include "pairs.hpp"

class datasetClass {
    public:

    struct vectorHash {
        size_t operator()(const std::vector<int> &vec) const {

            size_t hash = 0;

            for (const auto &i : vec) {
                hash ^= std::hash<int>()(i);
            }

            return hash;
        }
    };

    std::unordered_map<std::string, int> words;
    std::unordered_map<std::vector<int>, int, vectorHash> tokenizedWords;

    void parse(std::string &filePath) {

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

                        words[word]++;

                        // // Tokenize the word
                        // std::vector<int> tokens = trie.tokenize(word);

                        // // Create the pairs
                        // if (tokens.size() > 1) {
                        //     pairs.createPair(tokens);
                        // }

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

                words[word]++;

                // // Tokenize the word
                // std::vector<int> tokens = trie.tokenize(word);

                // // Create the pairs
                // if (tokens.size() > 1) {
                //     pairs.createPair(tokens);
                // }
            }
        }
    }
};