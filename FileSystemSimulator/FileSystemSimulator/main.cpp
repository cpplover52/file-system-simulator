#include "file_system.h"
#include "process_command.h"
#include <iostream>
#include <string>
using namespace std;

void PrintTransactions() {
	cout << "***Islemler***\n";
	cout << "0 -> Cikis\n";
	cout << "1 -> Dosya Olustur\n";
	cout << "2 -> Klasor Olustur\n";
	cout << "3 -> Dosya/Klasor Sil\n";
	cout << "4 -> Dosya/Klasor Adini Degistir\n";
	cout << "5 -> Konum Degistir\n";
}
//void CreateFile(){
//	string fileName;
//	bool isFolder = false;
//	vector<FileNode> childFiles = {};
//	cout << "Olusturulacak dosya adini girin: ";
//	cin >> fileName;
//	FileNode newFileNode = {fileName, isFolder, childFiles};
//}
//void CreateFolder() {
//	string fileName;
//	bool isFolder = true;
//	vector<FileNode> childFiles = {};
//	cout << "Olusturulacak klasor adini girin: ";
//	cin >> fileName;
//	FileNode newFileNode = { fileName, isFolder, childFiles };
//}
//void DeleteFile(){
//	short fileNumber;
//	int childCount = currentFileNode.childFiles.size();
//	for (int i = 0; i < childCount; i++) {
//		cout << i + 1 << " -> " << currentFileNode.childFiles[i].fileName << endl;
//	}
//	cout << "Silinecek dosya numarasýný girin: ";
//	cin >> fileNumber;
//}
//void PrintFileNode(FileNode fileNode) {
//	cout << "***Dosya Bilgileri***\n";
//	cout << "Dosya Adi: " << fileNode.fileName << "\Klasor Mu: " << fileNode.isFolder << "\nAlt Dosyalar: {";
//	int childCount = fileNode.childFiles.size();
//	for (int i = 0; i < childCount; i++) {
//		if (i == childCount - 1) {
//			cout << fileNode.childFiles[i].fileName << " }";
//		}
//		else {
//			cout << fileNode.childFiles[i].fileName << ", ";
//		}
//	}
//	cout << "\n******\n";
//}
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
	//FileSystemManager fsm;
	//short input = 1;
	//while (input != 0) {
	//	cout << "===============\n";
	//	fsm.printCurrentPath();
	//	fsm.list();
	//	PrintTransactions();
	//	cout << "===============\n";
	//	cout << "Bir islem secin: ";
	//	cin >> input;
	//	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Buffer'dan \n siliniyor
	//	switch (input)
	//	{
	//	case 0:
	//		cout << "Cikis yapiliyor....\n\n";
	//		return 0;
	//	case 1: {
	//		string fileName;
	//		cout << "Dosya adi girin: ";
	//		getline(cin, fileName); // Dosya adý boþluk içerebilir.
	//		fsm.createFile(fileName);
	//		break;
	//	}
	//	case 2: {
	//		string folderName;
	//		cout << "Klasor adi girin: ";
	//		getline(cin, folderName); // Klasör adý boþluk içerebilir.
	//		fsm.createFolder(folderName);
	//		break;
	//	}
	//	case 3: {
	//		fsm.list();
	//		string fileName;
	//		cout << "Silmek istediðiniz dosya/klasor adini girin: ";
	//		getline(cin, fileName);
	//		fsm.deleteFile(fsm.getCurrent(), fileName);
	//		break;
	//	}
	//	case 4: {
	//		fsm.list();
	//		string fileName, newFileName;
	//		cout << "Adini degistirmek istediðiniz dosya/klasor adini girin: ";
	//		getline(cin, fileName);
	//		cout << "Dosyanin yeni adini girin: ";
	//		getline(cin, newFileName);
	//		fsm.renameFile(fileName, newFileName);
	//		break;
	//	}
	//	case 5: {
	//		fsm.list();
	//		string folderName;
	//		cout << "Gitmek istediðiniz klasorun adini girin(.. ust klasore gider):";
	//		getline(cin, folderName);
	//		if (folderName == "..") {
	//			fsm.cdUp();
	//		}
	//		else {
	//			fsm.cd(folderName);
	//		}
	//		break;
	//	}
	//	default:
	//		break;
	//	}
	//}
	return 0;
}