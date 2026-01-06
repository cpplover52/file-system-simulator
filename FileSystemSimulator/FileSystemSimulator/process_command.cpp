#include "process_command.h"
#include <string>
#include <iostream>
#include <sstream>
using std::string;
using std::cout;
using std::cin;
void processCommand(FileSystemManager& fsm, const string& input) {
    string command;
    string argument;
    stringstream ss(input);
    ss >> command;
    getline(ss, argument);
    if (!argument.empty() && argument[0] == ' ') {
        argument.erase(0, 1);
    }
    if(command == "help") {
        fsm.printCommands();
    }
    else if (command == "finfo") {
        FileNode* file = fsm.findFile(fsm.getCurrent(), argument);
        fsm.showFileInfos(file);
    }
    else if (command == "cfi") {
        fsm.createFile(argument);
    }
    else if (command == "cfo") {
        fsm.createFolder(argument);
    }
    else if (command == "df") {
        fsm.deleteFile(fsm.getCurrent(), argument);
    }
    else if (command == "ref") {
        string newName;
        cout << "Yeni adi girin: ";
        getline(cin, newName);
        fsm.renameFile(argument, newName);
    }
    else if (command == "cl") {
        if (argument == "..") {
            fsm.cdUp();
        }
        else {
            fsm.cd(argument);
        }
    }
    else if (command == "ls") {
        fsm.list();
    }
    else if (command == "exit") {
        exit(0);
    }
    else {
        cout << "Gecersiz komut. Komutlarý gormek icin 'help' yazin.\n";
    }
}