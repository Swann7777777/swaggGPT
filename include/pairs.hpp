#pragma once

#include <unordered_map>
#include <vector>

class pairsClass {
    public:

    struct pairHash {
        size_t operator()(const std::pair<int, int> &pair) const {

            size_t hash1 = std::hash<int>{}(pair.first);
            size_t hash2 = std::hash<int>{}(pair.second);

            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };

    std::unordered_map<std::pair<int, int>, int, pairHash> frequencies;

    void createPair(std::vector<int> tokens) {

        for (int i = 0; i < tokens.size() - 1; i++) {

            frequencies[{tokens[i], tokens[i + 1]}]++;
        }
    }
};