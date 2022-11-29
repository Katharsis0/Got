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

#include "CommandParser.h"
std::string setPath;
using namespace std;
///Util
CommandParser::CommandParser(int &argc, char **argv) {
    if(argc>20){
        throw std::runtime_error("Too much parameters!\n\n"
                                 "For usage information type \e[1mgot help\e[0m\n\n");
    }
    else{
    for(int i=0; i< argc; ++i){
        this->tokens.push_back(std::string(argv[i]));
    }}
}
const std::string& CommandParser::getOption(const std::string &option) const {
    std::vector<std::string>::const_iterator itr;
    itr= std::find(this->tokens.begin(), this->tokens.end(), option);
    if(itr!= this->tokens.end() && ++itr!= this->tokens.end()){
        return *itr;
    }
    static const std::string emptyString("");
    return emptyString;
}
bool CommandParser::optionExists(const std::string &option) const {
    return std::find(this->tokens.begin(), this->tokens.end(), option)!=this->tokens.end();
}


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

string post_request(const string& url ,const string& path2file)
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
    catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what() << std::endl;
    }

}

json getLastCommit(string file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    return get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + file + "/lastcommit");

}

int verificarArchivo(const string& file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    string mensaje = "EXISTS(SELECT 1 FROM " + (string)r["nameRepo"] +"_versiones WHERE archivo = '" + file + "')";

    return (get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + file + "/exist"))[0][mensaje];

}


json getID(int id){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    return get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + to_string(id));

}

///Options
void CommandParser::rollbackOption (const string& commit, const string& file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    string get = "http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + file + "/" + commit;

    json commite = get_request(get);
    int id = commite[0]["id"];

    json rest = get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + to_string(id) + "/getData")[0];

    std::ofstream outfile (rest["archivo"]);

    string sTabla = rest["contenido"];

    json tabla = json::parse(sTabla);

    unordered_map<string, string> mapaDesdeJson = jsonHaciaTabla(tabla);

    string texto = descomprimirTabla(mapaDesdeJson, rest["comprimido"]);

    outfile << texto;

    outfile.close();
}

void CommandParser::resetOption(const string& file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    json lastCommitVersionFile = getID(getLastCommit(file)[0]["MAX(id)"]);

    int id = lastCommitVersionFile[0]["id"];

    json rest = get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + to_string(id) + "/getData")[0];

    std::ofstream outfile (rest["archivo"]);

    string sTabla = rest["contenido"];

    json tabla = json::parse(sTabla);

    unordered_map<string, string> mapaDesdeJson = jsonHaciaTabla(tabla);

    string texto = descomprimirTabla(mapaDesdeJson, rest["comprimido"]);

    outfile << texto;

    outfile.close();

}

void CommandParser::statusOption(){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    json lastID = getID(get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/lastID")[0]["MAX(id)"]);
    string commit = lastID[0]["commit"];

    cout << get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + commit + "/status") << "\n";


}

void CommandParser::statusOption(string file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    cout << get_request("http://127.0.0.1:5000/" + (string)r["nameRepo"] + "/" + file + "/statusFile") << "\n";

}


void CommandParser::helpOption() {
    printf("These are the commands implemented by Got:\n");
        printf("\e[1mgot init <-n> <name>\e[0m\n  Initializes the repository in the server.\n  "
               "Generates a .gotignore file where you can specify\n  "
               "which files you want to exclude from the repository.\n\n");
        printf("\e[1mgot help\e[0m\n  Displays the command information.\n\n");
        printf("\e[1mgot add [-fns] [filename(s)]\e[0m\n  Allows to add files to the repository.\n  "
               "You can use it either without the flag parameter to add all local files or specify the filenames\n  "
               "you want to add after the -fns flag.\n\n");
        printf("\e[1mgot commit <message>\e[0m\n  Submits to the server all the files previously added\n  "
               "to the repository with a brief description message.\n  "
               "In order to successfully commit, the local version of the repository must\n  "
               "be up-to-date with the global repository.\n\n");
        printf("\e[1mgot status <filename>\e[0m\n  "
               "Shows the last changes made to the specified file in regards to the last commit.\n  "
               "If a <filename> is specified, shows the change history of that particular file.\n\n");
        //TODO: Definir si el commit va a ser por #commit o por commit id (MD5).
        printf("\e[1mgot rollback <filename> <commit>\e[0m\n  "
               "Reverts the changes made to a file to the version of the specified past commit.\n\n");
        printf("\e[1mgot reset <filename>\e[0m\n  "
               "Reverts the local changes to the last commit.\n\n");
        printf("\e[1mgot sync <file>\e[0m\n  "
               "Synchronizes the specified file with its version in the global repository.\n\n");
}

void CommandParser::initOption(std::string repoName){

    std::ofstream outFile(".gotignore");
    outFile << "./gotignore" << std::endl;
    outFile.close();

    std::ofstream repo("repositorio.json");
    json info;
    info["nameRepo"] = repoName;
    info["version"] = {};
    repo << info;
    repo.close();

    std::ofstream add("add.json");
    json ad;
    ad["Lista"] = {};
    add << ad;
    add.close();

    std::ofstream file ("send.json");
    json j;
    j["name"] = repoName;
    file << j;
    file.close();
    //ToDo: post request
    printf("Repository initialized successfully. Repository name: %s\n\n", repoName.c_str());
}


void CommandParser::commitOption (std::string commit){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    std::ifstream add("add.json");
    json j;
    j << add;
    add.close();

    for (auto &it : j["Lista"]) {
        if (verificarArchivo(it["name"]) == 1){

            if(verificarVersion(it["name"]) == 1){

                std::ofstream file("send.json");
                json jl;
                jl["name"] = it["name"];
                jl["nameRepo"] = r["nameRepo"];
                jl["commit"] = commit;

                Node* HuffmanTree = crearArbolDeHuffman(getText(it["name"]));
                //TABLA DEL ÁRBOL
                unordered_map<char, string> huffmanCode;
                unordered_map<char, string> huffmanTable = crearTabla(HuffmanTree, "", huffmanCode);
                //COMPRIMIDO
                string textoComprimido = comprimirTabla(huffmanTable, getText(it["name"]));
                //tabla a json
                json t = tablaHaciaJson(huffmanTable);

                jl["contenido"] = t.dump();
                jl["comprimido"] = textoComprimido;

                file << jl;
                file.close();
                post_request("http://127.0.0.1:5000/add", "send.json");

                for (auto &i : r["version"]) {
                    if (it["name"] == i["name"]) {
                        i["version"] = getLastCommit(i["name"])[0]["MAX(id)"];
                    }
                }

            }else{
                cout << "El archivo " << it["name"] << " no está en su ultimo commit" << endl;
            }

        }
        else{

            std::ofstream file("send.json");
            json jk;
            jk["name"] = it["name"];
            jk["nameRepo"] = r["nameRepo"];
            jk["commit"] = commit;

            Node* HuffmanTree = crearArbolDeHuffman(getText(it["name"]));
            //TABLA DEL ÁRBOL
            unordered_map<char, string> huffmanCode;
            unordered_map<char, string> huffmanTable = crearTabla(HuffmanTree, "", huffmanCode);
            //COMPRIMIDO
            string textoComprimido = comprimirTabla(huffmanTable, getText(it["name"]));
            //tabla a json
            json t = tablaHaciaJson(huffmanTable);

            jk["contenido"] = t.dump();
            jk["comprimido"] = textoComprimido;

            file << jk;
            file.close();
            post_request("http://127.0.0.1:5000/add", "send.json");

            r["version"] += {{"name" , it["name"]}, {"version", getLastCommit(it["name"])[0]["MAX(id)"]}};

        }

    }

    std::ofstream addi("add.json");
    json ad;
    ad["Lista"] = {};
    addi << ad;
    add.close();

    std::ofstream repos("repositorio.json");
    repos << r;
    repos.close();


}




