#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    TrieNode() { isEndOfWord = false; }
};

class Trie {
private:
    TrieNode* root;
    // Yardýmcý fonksiyon: Verilen düðümden aþaðýya inip kelimeleri toplar (DFS)
    void getSuggestionsRecursive(TrieNode* node, string currentPrefix, vector<string>& results) {
        if (node->isEndOfWord) {
            results.push_back(currentPrefix);
        }
        for (auto pair : node->children) {
            getSuggestionsRecursive(pair.second, currentPrefix + pair.first, results);
        }
    }

public:
    Trie() { root = new TrieNode(); }
    void insert(string word) {
        TrieNode* temp = root;
        for (char c : word) {
            if (temp->children.find(c) == temp->children.end()) {
                temp->children[c] = new TrieNode();
            }
            temp = temp->children[c];
        }
        temp->isEndOfWord = true;
    }

    vector<string> getSuggestions(string prefix) {
        TrieNode* temp = root;
        vector<string> results;
        for (char c : prefix) {
            if (temp->children.find(c) == temp->children.end()) {
                return results; 
            }
            temp = temp->children[c];
        }
        getSuggestionsRecursive(temp, prefix, results);
        return results;
    }
};