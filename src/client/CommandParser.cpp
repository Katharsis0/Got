//
// Created by katharsis on 11/14/22.
//

#include "../include/CommandParser.hpp"
// Util
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

///Options
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
    //ToDo: Submitear al server el repo
    std::ofstream outFile(".gotignore");
    //ToDo: Agregar informacion al gotignore (por el momento escribe el nombre del repo)
    outFile << repoName.c_str() << std::endl;
    outFile.close();
    printf("Repository initialized successfully. Repository name: %s\n\n", repoName.c_str());
}

void CommandParser::addOption(std::string fileName){

}




