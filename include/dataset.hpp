#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>

class datasetClass {
    public:

    std::unordered_map<std::string, int> words;

    void parse(std::ifstream &file) {

        std::string line;

        while (getline(file, line)) {
            std::stringstream lineStream(line);

            std::string word;

            while (lineStream >> word) {
                words[word]++;
            }
        }
    }
};