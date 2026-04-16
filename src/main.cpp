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
    int chunkSize = 10000;

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

    std::mutex updateMutex;

    // Generate new tokens until the goal is reached
    while (static_cast<int>(vocabulary.tokens.size()) < vocabularySizeGoal) {

        auto pairStart = std::chrono::high_resolution_clock::now();

        auto it = dataset.tokenizedWords.begin();

        for (int i = 0; i < threadPool.threads.size(); i++) {

            auto start = it;
            auto end = start;

            int count = 0;

            while (count < chunkSize && end != dataset.tokenizedWords.end()) {
                ++end;
                ++count;
            }

            if (i == threadPool.threads.size() - 1) {
                end = dataset.tokenizedWords.end();
            }


            threadPool.enqueue([&pairs, start, end, &updateMutex] {

                std::unordered_map<std::pair<int, int>, int, pairsClass::pairHash> tmpPairs;

                for (auto it = start; it != end; it++) {
                    pairs.createPair(it->first, it->second, tmpPairs);
                }

                {
                    std::lock_guard<std::mutex> lock(updateMutex);
                    for (const auto &[pair, frequency] : tmpPairs) {
                        pairs.pairs[pair] += frequency;
                    }
                }
            });

            it = end;
        }

        threadPool.wait();

        auto pairEnd = std::chrono::high_resolution_clock::now();
        auto pairDuration = std::chrono::duration_cast<std::chrono::milliseconds>(pairEnd - pairStart);

        std::cout << pairDuration.count() << " ms\n";


        // Find the most frequent pair
        pairs.count();

        // Add the most frequent pair to the vocabulary
        vocabulary.output(pairs, vocabularyFilePath);

        // Clear the pairs
        pairs.pairs.clear();

        // Declare temporary vector for unordered map update
        std::unordered_map<std::vector<int>, bool, datasetClass::vectorHash> oldTokenizedWords;
        std::vector<std::pair<std::vector<int>, int>> newTokenizedWords;

        it = dataset.tokenizedWords.begin();

        for (int i = 0; i < threadPool.threads.size(); i++) {

            auto start = it;
            auto end = start;

            int count = 0;

            while (count < chunkSize && end != dataset.tokenizedWords.end()) {
                ++end;
                ++count;
            }

            if (i == threadPool.threads.size() - 1) {
                end = dataset.tokenizedWords.end();
            }

            threadPool.enqueue([start, end, &updateMutex, &pairs, &vocabulary, &newTokenizedWords, &oldTokenizedWords] {

                std::vector<std::pair<std::vector<int>, int>> tmpNewTokenizedWords;
                std::vector<std::vector<int>> tmpOldTokenizedWords;

                for (auto it = start; it != end; it++) {

                    std::vector<int> tokens = it->first;
                    int frequency = it->second;

                    // The tokenized word is only one token long
                    if (tokens.size() <= 1) {
            
                        // Throw it away
                        tmpOldTokenizedWords.push_back(tokens);
            
                        continue;
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
            
                        // Add the new and old words to the update vectors
                        tmpNewTokenizedWords.push_back({newTokens, frequency});
                        tmpOldTokenizedWords.push_back(tokens);
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(updateMutex);
                    for (const auto &i : tmpOldTokenizedWords) {
                        oldTokenizedWords[i] = true;
                    }
                    newTokenizedWords.insert(newTokenizedWords.end(), tmpNewTokenizedWords.begin(), tmpNewTokenizedWords.end());
                }
            });

            it = end;
        }


        threadPool.wait();

        // Remove old tokenized words
        for (const auto &i : oldTokenizedWords) {
            dataset.tokenizedWords.erase(i.first);
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