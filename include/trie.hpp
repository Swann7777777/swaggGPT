#pragma once

#include <string>
#include <vector>

class trieClass {
    public:

    class node {
        public:
        
        node* children[26] = {nullptr};

        int count = 0;
    };

    node* root;

    void insert(std::string word, int count) {

        // The insertion starts at the root
        node* currentNode = root;

        for (const auto &c : word) {

            // Check if the current node has the character
            if (currentNode->children[c - 'a'] == nullptr) {

                // Add the character to the node
                currentNode->children[c - 'a'] = new node();
            }

            // Advance in the tree
            currentNode = currentNode->children[c - 'a'];
        }

        // Add the word count to the last character
        currentNode->count = count;
    }


    trieClass(std::vector<std::string> words) {

        // Create the root node
        root = new node();

        // Generate the tree
        for (int i = 0; i < static_cast<int>(words.size()); i++) {
            insert(words[i], i);
        }
    }
};