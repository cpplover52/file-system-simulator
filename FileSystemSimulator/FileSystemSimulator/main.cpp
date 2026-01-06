#include "file_system.h"
#include "process_command.h"
#include <iostream>
#include <string>
using namespace std;

int main() {
	setlocale(LC_ALL, "Turkish");
	srand(time(NULL));
	FileSystemManager fsm;
	string input;
	while (true) {
		fsm.printCurrentPath();
		getline(cin,input);
		processCommand(fsm, input);
	}
	return 0;
}