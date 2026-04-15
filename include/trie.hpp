#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class trieClass {
    public:

    class node {
        public:
        
        std::unordered_map<char, std::unique_ptr<node>> children;

        int token = -1;
    };

    std::unique_ptr<node> root;

    void insert(std::string token, int index) {

        // The insertion starts at the root
        node* currentNode = root.get();

        // Iterate through the token's characters
        for (const auto &c : token) {

            // If the current node doesn't have the current character, add it
            if (!currentNode->children.count(c)) {

                currentNode->children[c] = std::make_unique<node>();
            }

            // Advance in the tree
            currentNode = currentNode->children[c].get();
        }

        // Add the token index to the last character
        currentNode->token = index;
    }

    std::vector<int> tokenize(const std::string &word) {

        std::vector<int> tokens;

        node* currentNode = root.get();

        // Initialize variables to sentinel values
        int lastToken = -1;
        int lastIndex = -1;


        // Loop through the current word's characters
        for (int i = 0; i < static_cast<int>(word.size());) {

            // The current node contains the current character
            if (currentNode->token >= 0) {

                // Set the temporary variables to the current node information
                lastIndex = i;
                lastToken = currentNode->token;
            }

            // The current node has a child that contains the current character
            if (currentNode->children.count(word[i])) {

                // Advance in the tree
                currentNode = currentNode->children[word[i]].get();
                i++;
            }

            // The current node doesn't have a child that contains the current character
            else {

                // The current node contains the current character
                if (currentNode->token >= 0) {

                    // Add the current node's token to the tokens list and start back at the root of the tree
                    tokens.push_back(currentNode->token);
                    currentNode = root.get();
                }

                // The current node doesn't contain the current character
                else {

                    // Add the last known token to the tokens list, go back to the last valid character and start back at the root of the tree
                    tokens.push_back(lastToken);
                    i = lastIndex;
                    currentNode = root.get();
                }
            }

            // The current character is at the end of the word
            if (i == static_cast<int>(word.size())) {

                // The current node has the current character
                if (currentNode->token >= 0) {

                    // Add the current node's token to the tokens list
                    tokens.push_back(currentNode->token);
                }

                // The current node doesn't have the current character
                else {

                    // Add the last known token to the tokens list, go back to the last valid character and start back at the root of the tree
                    tokens.push_back(lastToken);
                    i = lastIndex;
                    currentNode = root.get();
                }
            }
        }

        return tokens;
    }

    void generate(std::vector<std::string> tokens) {

        // Generate the tree
        for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
            insert(tokens[i], i);
        }
    }


    trieClass() {

        // Create the root node
        root = std::make_unique<node>();
    }
};