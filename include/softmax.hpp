#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

class softmaxClass {
    public:

    enum direction {
        right,
        left
    };

    struct nodeStruct {

        std::unique_ptr<nodeStruct> rightChild = nullptr;
        std::unique_ptr<nodeStruct> leftChild = nullptr;

        float rightWeight = 0;
        float leftWeight = 0;

        int token = -1;
        int frequency = 0;
    };

    struct pathStruct {

        std::vector<nodeStruct*> path;
        std::vector<direction> directions;
    };


    static bool pairFrequencyComp(const std::pair<int, int> &a, const std::pair<int, int> &b) {

        return a.second > b.second;
    }

    static bool nodeFrequencyComp(const std::unique_ptr<nodeStruct> &a, const std::unique_ptr<nodeStruct> &b) {

        return a->frequency > b->frequency;
    }

    
    std::unique_ptr<nodeStruct> root;

    
    void buildTree(std::unordered_map<int, int> tokenFrequencies) {
        
        // Vector that will hold the sorted token frequencies from the map as pairs of token/frequency
        std::vector<std::pair<int, int>> sortedTokenFrequencies;

        // Copy the content of map into the vector
        std::copy(tokenFrequencies.begin(), tokenFrequencies.end(), std::back_inserter(sortedTokenFrequencies));

        // Sort the tokens by their frequency in descencing order
        std::sort(sortedTokenFrequencies.begin(), sortedTokenFrequencies.end(), pairFrequencyComp);

        // Vector of nodes that will hold the leaves of the tree
        std::vector<std::unique_ptr<nodeStruct>> leaves;

        // Generate all the leaves from the vector
        for (const auto &i : sortedTokenFrequencies) {

            std::unique_ptr<nodeStruct> newLeaf = std::make_unique<nodeStruct>();

            newLeaf->token = i.first;
            newLeaf->frequency = i.second;

            leaves.push_back(std::move(newLeaf));
        }

        // Build the tree from the bottom up with the huffman coding algorithm
        while (leaves.size() > 1) {

            std::unique_ptr<nodeStruct> newNode = std::make_unique<nodeStruct>();

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

    std::unordered_map<int, pathStruct> paths;

    // Searches recursively the node it's given, stops if it encounters a leaf
    void search(std::unique_ptr<nodeStruct> &currentNode, pathStruct currentPath) {

        // The current node is a leaf
        if (currentNode->token > -1) {

            // Add the current path to the paths map
            paths[currentNode->token] = currentPath;

            return;
        }
        
        // Update the current path to add the right child node
        currentPath.path.push_back(currentNode->rightChild.get());
        currentPath.directions.push_back(right);

        // Continue the search on the right child node
        search(currentNode->rightChild, currentPath);

        // Updaye the current path to add the left child node
        currentPath.path.back() = currentNode->leftChild.get();
        currentPath.directions.back() = left;

        // Continue the search on the left child node
        search(currentNode->leftChild, currentPath);
    }


    void buildPaths() {

        pathStruct path;

        search(root, path);
    }


    softmaxClass() {

        root = std::make_unique<nodeStruct>();
    }
};