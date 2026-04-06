#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>

class datasetClass {
    public:

    std::unordered_map<std::string, int> data;

    void parse(std::ifstream &file) {

        std::string line;

        while (getline(file, line)) {
            
        }
    }
};