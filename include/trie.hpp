#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class trieClass {
    public:

    class node {
        public:
        
        std::unordered_map<char, node*> childrens;

        int index = 0;
    };

    node* root;

    void insert(std::string token, int index) {

        // The insertion starts at the root
        node* currentNode = root;

        for (const auto &c : token) {

            // Check if the current node has the character
            if (currentNode->childrens[c] == nullptr) {

                // Add the character to the node
                currentNode->childrens[c] = new node();
            }

            // Advance in the tree
            currentNode = currentNode->childrens[c];
        }

        // Add the token index to the last character
        currentNode->index = index;
    }

    void traverse(std::string token) {

    }

    void generate(std::vector<std::string> tokens) {

        // Generate the tree
        for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
            insert(tokens[i], i);
        }
    }


    trieClass() {

        // Create the root node
        root = new node();
    }
};