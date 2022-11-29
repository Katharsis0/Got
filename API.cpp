//
// Created by katharsis on 11/29/22.
//
#include "API.h"

///Huffman
Node* getNode(char ch, int freq, Node* left, Node* right){
    Node* node = new Node();

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}
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

///API
std::string getText(const std::string& url){

    std::ifstream file;
    file.open (url);

    std::string text;
    std::string temp;

    while (std::getline(file, temp)){
        text += temp;
    }
    file.close();

    return text;
}
string post_request(const string& url, const string& path2file)
{
    const string field_divider="&";
    stringstream result;
    try
    {
        // This block responsible for reading in the fastest way media file
        //      and prepare it for sending on API server
        ifstream is(path2file);
        is.seekg(0, ios_base::end);
        size_t size=is.tellg();
        is.seekg(0, ios_base::beg);
        vector<char> v(size/sizeof(char));
        is.read((char*) &v[0], size);
        is.close();
        string body(v.begin(), v.end());

        // Initialization
        curlpp::Cleanup cleaner;
        curlpp::Easy request;
        list< string > headers;
        headers.push_back("Content-Type: application/json");
        headers.push_back("User-Agent: curl/7.77.7");

        using namespace curlpp::Options;

        request.setOpt(new Verbose(true));
        request.setOpt(new HttpHeader(headers));
        request.setOpt(new Url(url));
        request.setOpt(new PostFields(body));
        request.setOpt(new PostFieldSize(body.length()));
        request.setOpt(new curlpp::options::SslEngineDefault());
        request.setOpt(WriteStream(&result));
        request.perform();
    }
    catch ( curlpp::LogicError & e )
    {
        cout << e.what() << endl;
    }
    catch ( curlpp::RuntimeError & e )
    {
        cout << e.what() << endl;
    }

    return (result.str());

}
json get_request(const string& url)
{
    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        json resp;

        using namespace curlpp::Options;

        request.setOpt(Verbose(true));
        request.setOpt(Url(url));

        std::ostringstream json ;
        request.setOpt(new curlpp::options::WriteStream(&json));

        request.perform();

        resp = json::parse(json.str());

        return resp;
    }
    catch ( curlpp::LogicError & e ) {
        std::cout << e.what() << std::endl;
    }
}
json getLastCommit(const string& file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    return get_request("http://localhost:3000/" + (string)r["nameRepo"] + "/" + file + "/lastcommit");

}
int verificarArchivo(const string& file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    string mensaje = "EXISTS(SELECT 1 FROM " + (string)r["nameRepo"] +"_versiones WHERE archivo = '" + file + "')";

    return (get_request("http://localhost:3000/" + (string)r["nameRepo"] + "/" + file + "/exist"))[0][mensaje];

}
bool verificarVersion(const string& file){
    int versionGlobal = getLastCommit(file)[0]["MAX(id)"];

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    int versionLocal;
    for (auto &it : r["version"]){
        cout << file << "\n";
        cout << (string)it["name"] << "\n";
        if (file == ((string)it["name"])){
            versionLocal = (int)it["version"];
            cout << "VERSION local: " << versionLocal;
        }
    }

    if((versionLocal != 0) && (versionLocal == versionGlobal)){
        return 1;
    }else{
        return 0;
    }
}
json getID(int id){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    return get_request("https://localhost:3000/" + (string)r["nameRepo"] + "/" + to_string(id));

}
