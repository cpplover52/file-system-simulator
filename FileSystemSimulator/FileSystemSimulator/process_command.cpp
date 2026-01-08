#include "process_command.h"
#include <string>
#include <iostream>
#include <sstream>
using std::string;
using std::cout;
using std::cin;
using std::stringstream;

void processCommand(FileSystemManager& fsm, const string& input) {
	stringstream ss(input);
	string command, arg1, arg2;

	// Ýlk kelime komut, diðerleri argüman
	ss >> command >> arg1 >> arg2;

	if (command == "help") {
		fsm.printCommands();
	}
	else if (command == "undo") {
		fsm.undo();
	}
	else if (command == "redo") {
		fsm.redo();
	}
	else if (command == "finfo") {
		FileNode* file = fsm.findFile(fsm.getCurrent(), arg1);
		if (file == nullptr) cout << "Dosya bulunamadi.\n";
		else fsm.showFileInfos(file);
	}
	else if (command == "cfi") {
		fsm.createFile(arg1);
	}
	else if (command == "search") {
		if (arg1 == "-bs") {
			fsm.searchFileBinary(arg2);
		}
		else if (arg1 == "-bfs") {
			fsm.searchFileBFS(arg2);
		}
		else if (arg1 == "-ls") {
			fsm.searchFileLinear(arg2);
		}
	}
	else if (command == "cfo") {
		fsm.createFolder(arg1);
	}
	else if (command == "df") { // Delete File
		fsm.deleteFile(fsm.getCurrent(), arg1);
	}
	else if (command == "ref") { // Rename File
		if (arg1.empty() || arg2.empty()) {
			cout << "Hata: Eksik parametre. Kullanim: ref [eskiAd] [yeniAd]\n";
		}
		else {
			fsm.renameFile(arg1, arg2);
		}
	}
	else if (command == "mv") { // Move File
		if (arg1.empty() || arg2.empty()) {
			cout << "Hata: Eksik parametre. Kullanim: mv [dosya] [hedef]\n";
		}
		else {
			fsm.moveFile(arg1, arg2);
		}
	}
	else if (command == "cl") {
		if (arg1 == "..") fsm.cdUp();
		else fsm.cd(arg1);
	}
	else if (command == "ls") {
		if (arg1 == "") fsm.list();
		else if (arg1 == "-size") fsm.listBySize();
		else if (arg1 == "-tree") fsm.listAsTree();
		else if (arg1 == "-name") fsm.listByName();
	}
	else if (command == "lsr") {
		fsm.listRecycleBin();
	}
	else if (command == "exit") {
		exit(0);
	}
	else {
		cout << "'" << command << "' gecersiz komut. 'help' yazin.\n";
	}
}