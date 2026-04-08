#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class trieClass {
    public:

    class node {
        public:
        
        std::unordered_map<char, node*> childrens;

        int index = -1;
    };

    node* root;

    void insert(std::string token, int index) {

        // The insertion starts at the root
        node* currentNode = root;

        // Iterate through the token's characters
        for (const auto &c : token) {

            // If the current node doesn't have the current character, add it
            if (!currentNode->childrens.count(c)) {

                currentNode->childrens[c] = new node();
            }

            // Advance in the tree
            currentNode = currentNode->childrens[c];
        }

        // Add the token index to the last character
        currentNode->index = index;
    }

    std::string traverse(std::string token) {

        // The tree traversal starts at the root
        node* currentNode = root;

        // Variable that holds the longest match
        std::string longestMatch = "";

        // Iterate through the token characters
        for (const auto &c : token) {

            // If the current node has the current character, advance in the tree
            if (currentNode->childrens.count(c)) {

                // Set the current node to its children with the character
                currentNode = currentNode->childrens[c];

                // Add the character to the longest match
                longestMatch += c;
            }

            // If the current node doesn't contain the current character, return the token index
            else {
                if (currentNode->index < 0) {
                    std::cerr << "Invalid index in trie for token " << token << "\n";
                    exit(1);
                }

                // Return the longest match
                return longestMatch;
            }
        }

        return longestMatch;
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