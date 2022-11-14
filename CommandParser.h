//
// Created by katharsis on 11/14/22.
//

#ifndef CLI_COMMANDPARSER_H
#define CLI_COMMANDPARSER_H

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


class CommandParser {
    public:
        CommandParser(int &argc, char **argv);

        const std::string& getOption(const std::string& option) const;

        bool optionExists(const std::string& option) const;

        //Options
        static void helpOption();
        static void initOption(std::string repoName);
        static void addOption(std::string files);
        static void commitOption(std::string message);
        static void statusOption(std::string filename);
        static void rollbackOption(std::string filename, int commit);
        static void resetOption(std::string filename);
        static void syncOption(std::string filename);



    private:
        std::vector<std::string> tokens;
};


#endif //CLI_COMMANDPARSER_H
