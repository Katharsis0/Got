#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <experimental/filesystem>

#include "CommandParser.h"
///Se compila con g++ main.cpp CommandParser.cpp API.cpp -o got -lcurl -lcurlpp -lstdc++fs en la terminal





namespace fs= std::experimental::filesystem;


int main(int argc, char* argv[]){

    ///Constructor para parsear los comandos
    CommandParser cmd(argc,argv);
    ///Command verification
    if(cmd.optionExists("help")){
       CommandParser::helpOption();
    }
    if(cmd.optionExists("init")){
        const std::string &repoName = cmd.getOption("-n");
        if(!repoName.empty()){
            CommandParser::initOption(repoName);
        }
    }

    if(cmd.optionExists("add")){

        const std::string &flag = cmd.getOption("");
        const std::string &fileNames = cmd.getOption("-fns");
        ///Si no se especifican archivos se agregan todos los archivos al Pending2Commit.txt.
        if(flag.empty() && fileNames.empty()){
            printf("Added all local files to the repository.\n\n");
            std::string path= std::experimental::filesystem::current_path();
            ///lista de todos los archivos en el directorio
            std::string allFiles;
//          std::ofstream outFile("pending2commit.txt");
            for(const auto& entry : fs::directory_iterator(path)){
                allFiles+=entry.path();
            }
            cout <<"Files added: " << allFiles;
            CommandParser::addOption(allFiles);
        }
        ///Si se especifican los files, solo se agregan estos al repo.
        if(!fileNames.empty() && flag.empty()){
//            printf("Files added to the repository: %s\n\n",fileNames.c_str());
//            std::string s="";
//            std::string path= std::experimental::filesystem::current_path();
//            std::ofstream outFile("pending2commit.txt");
//
//            for(auto x: fileNames){
//                if(x==','){
//                    outFile << s << std::endl;
//                    s="";
//                }
//                else{
//                    s+=x;
//                }
//            }
//            outFile <<s << std::endl;
//            outFile.close();
            CommandParser::addOption(fileNames);
        }
    }
    if(cmd.optionExists("commit")){
        const std::string &msg = cmd.getOption("-m");

        if(!msg.empty()){
            CommandParser::commitOption(msg);
        }
        else {
            printf("Error: In order to commit a brief message is required. Please try again");
        }
    }
    if(cmd.optionExists("status")){
        const std::string &flag = cmd.getOption("");
        const std::string &fileName = cmd.getOption("-fn");
        ///Si no se especifica un archivo, se da el status del repo.
        if(flag.empty() && fileName.empty()){
            CommandParser::statusOption();
        }
        if(!fileName.empty() && flag.empty()){
            CommandParser::statusOption(fileName);
        }
    }

    if (cmd.optionExists("rollback")){
        const std::string &fileName = cmd.getOption("-fn");
        const std::string &commit = cmd.getOption("-c");
        if(!fileName.empty() && !commit.empty()){
            CommandParser::rollbackOption(commit, fileName);
        }
        else{
            printf("Error: A file and a past commit version must be specified in order to rollback. Please try again.");
        }
    }
    if(cmd.optionExists("reset")){
        const std::string &fileName = cmd.getOption("-fn");
        if(!fileName.empty()){
            CommandParser::resetOption(fileName);
        }
        else{
            printf("Error: A file to reset must be specified.");
        }
    }
    if(cmd.optionExists("sync")){
        const std::string &fileName = cmd.getOption("-fn");
        if(!fileName.empty()){
            CommandParser::syncOption(fileName);
        }
        else{
            printf("Error: A file must be specified in order to sync it with the global repository.");
        }

    }
    return 0;
}
