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

    std::vector<int> tokenize(std::string &word) {

        std::vector<int> tokens;

        node* currentNode = root.get();

        int lastToken = -1;
        int lastIndex = -1;


        for (int i = 0; i < static_cast<int>(word.size());) {

            if (currentNode->token >= 0) {
                lastIndex = i;
                lastToken = currentNode->token;
            }



            if (currentNode->children.count(word[i])) {
                currentNode = currentNode->children[word[i]].get();
                i++;
            }

            else {

                if (currentNode->token >= 0) {
                    tokens.push_back(currentNode->token);
                    currentNode = root.get();
                }

                else {
                    tokens.push_back(lastToken);
                    i = lastIndex;
                    currentNode = root.get();
                }
            }

            if (i == static_cast<int>(word.size())) {

                if (currentNode->token >= 0) {
                    tokens.push_back(currentNode->token);
                }

                else {
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