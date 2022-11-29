//
// Created by katharsis on 11/14/22.
//

#include "CommandParser.h"

///Util
CommandParser::CommandParser(int &argc, char **argv) {
    if(argc>20){
        throw std::runtime_error("Too much parameters!\n\n"
                                 "For usage information type \e[1mgot help\e[0m\n\n");
    }
    else{
    for(int i=0; i< argc; ++i){
        this->tokens.emplace_back(argv[i]);
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

///Options
void CommandParser::helpOption() {
    printf("These are the commands implemented by Got:\n");
        printf("\e[1mgot init <-n> <name>\e[0m\n  Initializes the repository in the server.\n  "
               "Generates a .gotignore file where you can specify\n  "
               "which files you want to exclude from the repository.\n\n");
        printf("\e[1mgot help\e[0m\n  Displays the command information.\n\n");

        printf("\e[1mgot add <-fns> <filename(s)>\e[0m\n  Allows to add files to the repository.\n  "
               "You can use it either without the flag parameter to add all local files or specify the filenames\n  "
               "you want to add after the -fns flag.\n\n");

        printf("\e[1mgot commit -m <message>\e[0m\n  Submits to the server all the files previously added\n  "
               "to the repository with a brief description message.\n  "
               "In order to successfully commit, the local version of the repository must\n  "
               "be up-to-date with the global repository.\n\n");

        printf("\e[1mgot status <-fn> <filename>\e[0m\n  "
               "Shows the last changes made to the repository in regards to the last commit.\n  "
               "If a <filename> is specified after the <-fn> flag, it shows the change history of that particular file.\n\n");

        printf("\e[1mgot rollback -fn <filename> -c <commit>\e[0m\n  "
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
    post_request("https://localhost:3000/init" ,"send.json");
    printf("Repository initialized successfully. Repository name: %s\n\n", repoName.c_str());
}

void CommandParser::addOption(std::string fileName){
    std::ifstream add("add.json");
    json addi;
    addi <<add;
    json temp;
    temp["name"] = fileName;
    addi["Lista"]+= temp;
    add.close();

    ofstream add2("add.json");
    add2 <<addi;
    add2.close();

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
                post_request("https://localhost:3000/add" ,"send.json");

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
            post_request("https://localhost:3000/add" ,"send.json");

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

void CommandParser::statusOption(std::string fileName){
    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();
    cout << get_request("https://localhost:3000/status" + (string)r["nameRepo"] + "/" + fileName + "/statusFile") << "\n";
}

void CommandParser::statusOption(){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    json lastID = getID(get_request("https://localhost:3000/" + (string)r["nameRepo"] + "/lastID")[0]["MAX(id)"]);
    string commit = lastID[0]["commit"];

    cout << get_request("https://localhost:3000/" + (string)r["nameRepo"] + "/" + commit + "/status") << "\n";

}
void CommandParser::rollbackOption(const std::string commit, const std::string filename){
    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();

    string get = "https://localhost:3000/" + (string)r["nameRepo"] + "/" + filename + "/" + commit;

    json commite = get_request(get);
    int id = commite[0]["id"];

    json rest = get_request("https://localhost:3000/" + (string)r["nameRepo"] + "/" + to_string(id) + "/getData")[0];

    std::ofstream outfile (rest["archivo"]);
    string sTabla = rest["contenido"];
    json tabla = json::parse(sTabla);
    unordered_map<string, string> mapaDesdeJson = jsonHaciaTabla(tabla);
    string texto = descomprimirTabla(mapaDesdeJson, rest["comprimido"]);

    outfile << texto;

    outfile.close();
}

void CommandParser::syncOption(string& file){

    std::ifstream repo("repositorio.json");
    json r;
    r << repo;
    repo.close();
    string id = to_string(getLastCommit(file)[0]["MAX(id)"]);
    json rest = get_request("https://localhost:3000/" + (string)r["nameRepo"] + "/" + id + "/getData")[0];
    std::ofstream outfile (rest["archivo"]);
    string sTabla = rest["contenido"];
    json tabla = json::parse(sTabla);
    unordered_map<string, string> mapaDesdeJson = jsonHaciaTabla(tabla);
    string texto = descomprimirTabla(mapaDesdeJson, rest["comprimido"]);
    outfile << texto;
    outfile.close();
}

