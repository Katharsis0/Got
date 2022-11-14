#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <experimental/filesystem>

#include "CommandParser.h"

///Se compila con g++ main.cpp CommandParser.cpp -o got -lstdc++fs en la terminal

namespace fs= std::experimental::filesystem;

int main(int argc, char* argv[]){

    ///Constructor para parsear los comandos
    CommandParser cmd(argc,argv);
    ///Command verification
    if(cmd.optionExists("help")){
       CommandParser::helpOption();
    }

    if(cmd.optionExists("init")){
        //ToDo: Create n-ary tree for file handling
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
            std::string path= "/home/katharsis/CLionProjects/CLI";
            std::ofstream outFile("pending2commit.txt");
            for(const auto& entry : fs::directory_iterator(path)){
                outFile << entry.path() << std::endl;
            }
            outFile.close();
        }
        ///Si se especifican los files, solo se agregan estos al pending2commit.txt
        if(!fileNames.empty() && flag.empty()){
            printf("Files added to the repository: %s\n\n",fileNames.c_str());
            std::string s="";
            std::string path= "/home/katharsis/CLionProjects/CLI";
            std::ofstream outFile("pending2commit.txt");
            ///Escribe los archivos especificados separados por coma en el .txt
            for(auto x: fileNames){
                if(x==','){
                    outFile << s << std::endl;
                    s="";
                }
                else{
                    s+=x;
                }
            }
            outFile <<s << std::endl;
            outFile.close();
        }

    }

    return 0;
}