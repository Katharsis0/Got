//
// Created by katharsis on 11/29/22.
//

//Huffman
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <fstream>
#include "json.hpp"
#include <filesystem>
#include <experimental/filesystem>
#include "sha256.h"
#include <unordered_map>
#include <queue>
#include "json.hpp"
///Se debe incluir libreria de curl

using json = nlohmann::json;


struct Node
{
    char ch;
    int freq;
    Node *left, *right;
};

Node* getNode(char ch, int freq, Node* left, Node* right){
    Node* node = new Node();

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}

struct comp{
    bool operator()(Node* l, Node* r){
        // highest priority item has the lowest frequency
        return l->freq > r->freq;
    }
};

void decode(Node* root, int &index, string str){
    if (root == nullptr) {
        return;
    }
    if (!root->left && !root->right){
        return;
    }

    index++;

    if (str[index] =='0') {
        decode(root->left, index, str);
    } else {
        decode(root->right, index, str);
    }
}

string decompressedData(unordered_map<char, string> huffmanCode, string text){

    string valor;
    string textoInicial;

    for(int i = 0; i < text.length() ; i++){

        valor += text[i];

        for (auto pair: huffmanCode) {
            if(valor == pair.second){
                textoInicial += pair.first;
                valor = "";
                break;
            }
        }
    }


    return textoInicial;
}

Node* crearArbolDeHuffman(string text){
    unordered_map<char, int> freq;
    for (char ch: text) {
        freq[ch]++;
    }

    priority_queue<Node*, vector<Node*>, comp> pq;

    for (auto pair: freq) {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    while (pq.size() != 1){

        Node *left = pq.top(); pq.pop();
        Node *right = pq.top();	pq.pop();

        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    Node* root = pq.top();

    return root;
}

unordered_map<char, string> crearTabla(Node* root, string str, unordered_map<char, string> &huffmanCode){
    if (root == nullptr) {
        return huffmanCode;
    }
    // found a leaf node
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    crearTabla(root->left, str + "0", huffmanCode);
    crearTabla(root->right, str + "1", huffmanCode);

    return huffmanCode;
}

string comprimirTabla(unordered_map<char, string> huffmanCode, string text){
    string str = "";
    for (char ch: text) {
        str += huffmanCode[ch];
    }
    return str;
}

string descomprimirTabla(unordered_map<string, string> huffmanCode, string text){

    string valor;
    string textoInicial;

    for(int i = 0; i < text.length() ; i++){

        valor += text[i];

        for (auto pair: huffmanCode) {
            if(valor == pair.second){
                textoInicial += pair.first;
                valor = "";
                break;
            }
        }
    }

    return textoInicial;
}

json tablaHaciaJson(unordered_map<char, string> huffmanTable){
    json j;
    string s;

    for (auto pair: huffmanTable) {
        s.push_back(pair.first);
        j[s] = pair.second;
        s = "";
    }

    return j;
}

unordered_map<string, string> jsonHaciaTabla(json j){
    unordered_map<string, string> huffmanTable;

    std::vector<string> myvectorKey;
    std::vector<string> myvectorValue;

    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        myvectorKey.push_back(it.key());
        myvectorValue.push_back(it.value());
    }

    for (int i = 0; i < j.size(); i++) {
        huffmanTable[myvectorKey.at(i)] = myvectorValue.at(i);
    }

    return huffmanTable;
}