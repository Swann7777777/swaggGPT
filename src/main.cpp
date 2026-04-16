#include <fstream>
#include <string>
#include <chrono>
#include "dataset.hpp"
#include "vocabulary.hpp"
#include "trie.hpp"
#include "pairs.hpp"
#include "threads.hpp"

int main() {

    int vocabularySizeGoal = 1026;

    // std::string datasetFilePath("../resources/training/testTokens.txt");
    // std::string datasetFilePath("../resources/training/wikitext-103/wiki.test.tokens");
    std::string datasetFilePath("../resources/training/wikitext-103/wiki.train.tokens");
    std::string vocabularyFilePath("../resources/vocabulary.txt");

    // Initialize the thread pool
    threadPoolClass threadPool;

    // Load the vocabulary
    vocabularyClass vocabulary;
    vocabulary.load(vocabularyFilePath);

    // Create the trie data structure
    trieClass trie;
    trie.generate(vocabulary.tokens);

    // Initialize the pairs class
    pairsClass pairs;

    // Load the dataset
    datasetClass dataset;
    dataset.parse(datasetFilePath);

    auto start = std::chrono::high_resolution_clock::now();

    // Tokenize the entire dataset once
    for (const auto &[word, frequency] : dataset.words) {
        dataset.tokenizedWords[trie.tokenize(word)] += frequency;
    }

    // Generate new tokens until the goal is reached
    while (static_cast<int>(vocabulary.tokens.size()) < vocabularySizeGoal) {

        // Create the pairs from the tokenized words
        for (const auto &[tokens, frequency] : dataset.tokenizedWords) {
            pairs.createPair(tokens, frequency);
        }

        // Find the most frequent pair
        pairs.count();

        // Add the most frequent pair to the vocabulary
        vocabulary.output(pairs, vocabularyFilePath);

        // Clear the pairs
        pairs.pairs.clear();

        // Declare temporary vector for unordered map update
        std::vector<std::pair<std::vector<int>, int>> newTokenizedWords;
        std::vector<std::vector<int>> oldTokenizedWords;
        std::mutex updateMutex;

        // Find and edit the tokenized words that were affected by the newest vocabulary token
        for (const auto &[tokens, frequency] : dataset.tokenizedWords) {

            threadPool.enqueue([tokens, frequency, &oldTokenizedWords, &pairs, &vocabulary, &newTokenizedWords, &updateMutex] {

                // The tokenized word is only one token long
                if (tokens.size() <= 1) {
    
                    {
                        std::lock_guard<std::mutex> lock(updateMutex);
                        // Throw it away
                        oldTokenizedWords.push_back(tokens);
                    }

                    return;
                }
    
                // Declare a copy of the current word's tokens that can be modified
                std::vector<int> newTokens = tokens;
    
                // Iterate through the word's tokens
                for (int i = 0; i < tokens.size() - 1; i++) {
    
                    // Check the word contains the sequence
                    if (tokens[i] == pairs.mostFrequentPair.first && tokens[i + 1] == pairs.mostFrequentPair.second) {
    
                        // Replace the current token with the new one
                        newTokens[i] = vocabulary.tokens.size() - 1;
    
                        // Replace the second token of the pair with -1
                        newTokens[i + 1] = -1;
                    }
                }
                
                // Check if the word was affected by the newest token
                if (newTokens != tokens) {
                    
                    // Delete all temporary tokens in the new tokenized word
                    newTokens.erase(std::remove(newTokens.begin(), newTokens.end(), -1), newTokens.end());
    
                    {
                        std::lock_guard<std::mutex> lock(updateMutex);
                        // Add the new and old words to the update vectors
                        newTokenizedWords.push_back({newTokens, frequency});
                        oldTokenizedWords.push_back(tokens);
                    }
                }

                return;
            });
        }

        threadPool.wait();

        // Remove old tokenized words
        for (const auto &i : oldTokenizedWords) {
            dataset.tokenizedWords.erase(i);
        }

        // Add new tokenized words
        for (const auto &[tokens, frequency] : newTokenizedWords) {
            dataset.tokenizedWords[tokens] += frequency;
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << duration.count() << std::endl;

    return 0;
}