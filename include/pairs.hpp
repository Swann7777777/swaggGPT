#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>

class pairsClass {
    public:

    struct pairHash {
        size_t operator()(const std::pair<int, int> &pair) const {

            size_t hash1 = std::hash<int>{}(pair.first);
            size_t hash2 = std::hash<int>{}(pair.second);

            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };

    std::pair<int, int> mostFrequentPair;

    std::unordered_map<std::pair<int, int>, int, pairHash> pairs;

    void createPair(const std::vector<int> &tokens, const int &frequency, std::unordered_map<std::pair<int, int>, int, pairHash> &tmpPairs) {

        for (int i = 0; i < tokens.size() - 1; i++) {

            tmpPairs[{tokens[i], tokens[i + 1]}] += frequency;
        }
    }

    void count() {
        auto it = std::max_element(std::begin(pairs),
        std::end(pairs),
        [] (const std::pair<std::pair<int, int>, int> p1, const std::pair<std::pair<int, int>, int> p2) { return p1.second < p2.second; });

        if (it == pairs.end()) {
            std::cerr << "An error occured while finding the most frequent pair\n";
            exit(1);
        }

        mostFrequentPair = it->first;
    }
};