#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

class softmaxClass {
    public:

    struct node {

        std::unique_ptr<node> rightChild = nullptr;
        std::unique_ptr<node> leftChild = nullptr;

        float rightWeight = 0;
        float leftWeight = 0;

        int token = -1;
        int frequency = 0;
    };


    static bool pairFrequencyComp(const std::pair<int, int> &a, const std::pair<int, int> &b) {

        return a.second > b.second;
    }

    static bool nodeFrequencyComp(const std::unique_ptr<node> &a, const std::unique_ptr<node> &b) {

        return a->frequency > b->frequency;
    }

    
    std::unique_ptr<node> root;

    
    void buildTree(std::unordered_map<int, int> tokenFrequencies) {
        
        // Vector that will hold the sorted token frequencies from the map as pairs of token/frequency
        std::vector<std::pair<int, int>> sortedTokenFrequencies;

        // Copy the content of map into the vector
        std::copy(tokenFrequencies.begin(), tokenFrequencies.end(), std::back_inserter(sortedTokenFrequencies));

        // Sort the tokens by their frequency in descencing order
        std::sort(sortedTokenFrequencies.begin(), sortedTokenFrequencies.end(), pairFrequencyComp);

        // Vector of nodes that will hold the leaves of the tree
        std::vector<std::unique_ptr<node>> leaves;

        // Generate all the leaves from the vector
        for (const auto &i : sortedTokenFrequencies) {

            std::unique_ptr<node> newLeaf = std::make_unique<node>();

            newLeaf->token = i.first;
            newLeaf->frequency = i.second;

            leaves.push_back(std::move(newLeaf));
        }

        // Build the tree from the bottom up with the huffman coding algorithm
        while (leaves.size() > 1) {

            std::unique_ptr<node> newNode = std::make_unique<node>();

            newNode->rightChild = std::move(leaves.back());
            leaves.pop_back();

            newNode->leftChild = std::move(leaves.back());
            leaves.pop_back();

            // The frequency of the new node is set to the sum of the frequencies of the child nodes
            newNode->frequency = newNode->rightChild->frequency + newNode->leftChild->frequency;

            // Insert the newly created node in the correct position in the leaves vector to maintain ordering
            leaves.insert(std::upper_bound(leaves.begin(), leaves.end(), newNode, nodeFrequencyComp), std::move(newNode));
        }

        // Set the root as the last node remaining
        root = std::move(leaves[0]);
    }


    softmaxClass() {

        root = std::make_unique<node>();
    }
};