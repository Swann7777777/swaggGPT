#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>

class datasetClass {
    public:

    std::unordered_map<std::string, int> words;

    void parse(std::string &filePath) {

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "The dataset file could not be opened at path " << filePath << "\n";
            exit(1);
        }

        std::string line = "";

        
        // Iterate over dataset lines
        while (getline(file, line)) {
            
            // The current character should be ignored if this variable is true
            bool ignore = false;

            std::string word = "";

            // Iterate over line characters
            for (const auto &c : line) {
                
                if (c == '<') {
                    ignore = true;
                }

                else if (c == '>') {
                    ignore = false;
                }

                else if (c == ' ' || c == '.' || c == '-') {
                    if (word.size() != 0) {
                        words[word]++;
                        word = "";
                    }
                    continue;
                }

                else if (!ignore) {
                    if (std::isalpha(c)) {
                        word += static_cast<char>(std::tolower(c));
                    }
                }
            }
        }
    }
};