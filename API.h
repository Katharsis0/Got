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
#include <filesystem>
#include <experimental/filesystem>
//#include "sha256.h"
#include <unordered_map>
#include <queue>
#include "json.hpp"


using json = nlohmann::json;
using namespace std;

///Huffman Util
struct Node
{
    char ch;
    int freq;
    Node *left, *right;
};
Node* getNode(char ch, int freq, Node* left, Node* right);

struct comp{
    bool operator()(Node* l, Node* r){
        // highest priority item has the lowest frequency
        return l->freq > r->freq;
    }
};

void decode(Node* root, int &index, string str);
string decompressedData(unordered_map<char, string> huffmanCode, string text);
Node* crearArbolDeHuffman(string text);
unordered_map<char, string> crearTabla(Node* root, string str, unordered_map<char, string> &huffmanCode);
string comprimirTabla(unordered_map<char, string> huffmanCode, string text);
string descomprimirTabla(unordered_map<string, string> huffmanCode, string text);
json tablaHaciaJson(unordered_map<char, string> huffmanTable);
unordered_map<string, string> jsonHaciaTabla(json j);


///API post-requests
std::string getText(const std::string& url);
string post_request(const string& url, const string& path2file);
json get_request(const string& url);
json getLastCommit(const string& file);
json getID(int id);
int verificarArchivo(const string& file);
bool verificarVersion(const string& file);
