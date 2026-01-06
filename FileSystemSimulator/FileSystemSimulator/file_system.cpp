#include "file_system.h"
#include "sort_utils.h"
#include "time_utils.h"
#include <string>
#include <ctime>
#include <math.h>
FileSystemManager::FileSystemManager() {
	root = new FileNode{ {0, 0, calculateFullTime(),calculateFullTime()} ,"A:", true, {}, nullptr };
	current = root;
}
FileSystemManager::~FileSystemManager() {
	deleteFile(root->parent, root->fileName);
}
FileNode* FileSystemManager::findFile(FileNode* fileNode, const string& fileName) {
	auto it = find_if(fileNode->childFiles.begin(), fileNode->childFiles.end(),
		[&](FileNode* node) {
			return node->fileName == fileName;
		});
	if (it == fileNode->childFiles.end()) {
		return nullptr;
	}
	return *it;
}
long long FileSystemManager::generateSizeByExtension(const string& fileName) {
	// Basit bir hash veya son karakter kontrolü
	size_t dotPos = fileName.find_last_of(".");
	string ext = (dotPos != string::npos) ? fileName.substr(dotPos + 1) : "";

	// Rastgelelik için temel (srand main'de çaðrýlmýþ olmalý)
	if (std::find(textExtensions.begin(), textExtensions.end(), ext) != textExtensions.end()) {
		return 100 + (rand() % 900); // 100B - 1KB
	}
	else if (std::find(pictureExtensions.begin(), pictureExtensions.end(), ext) != pictureExtensions.end()) {
		return 1024 * (500 + (rand() % 2000)); // 500KB - 2.5MB
	}
	else if (std::find(videoExtensions.begin(), videoExtensions.end(), ext) != videoExtensions.end()) {
		return 1024LL * 1024 * (10 + (rand() % 490)); // 10MB - 500MB
	}
	else if (std::find(audioExtensions.begin(), audioExtensions.end(), ext) != audioExtensions.end()) {
		return 1024LL * 1024 * (1 + (rand() % 9)); // 1MB - 10MB
	}
	return rand() % 1024; // Bilinmeyenler için küçük boyut
}
void FileSystemManager::updateParentSizes(FileNode* node, long long sizeChange) {
	if (node == nullptr) return; // Kök dizinin üzerine çýkýldýysa dur

	node->fileInode.fileSize += sizeChange;

	if (node->parent != nullptr) {
		updateParentSizes(node->parent, sizeChange);
	}
}
void FileSystemManager::printFiles(vector<FileNode*> files) {
	int i = 1;
	for (auto file : files)
	{
		// Filename | (Folder/File) | Size
		string fileType, fullSizeStr, unitStr;
		SizeInfo si = getNormalizedSize(file->fileInode.fileSize);
		unitStr = getUnitStr(si.unit);
		fullSizeStr = std::to_string(si.value) + unitStr;
		if (file->isFolder) fileType = "folder";
		else fileType = "file";
		cout << i << ". " << file->fileName << "(" << fileType << ") | " << fullSizeStr << endl;
		i++;
	}
}
vector<FileNode*>::iterator FileSystemManager::findFileIterator(FileNode* parentNode, const string& fileName) {
	auto it = find_if(parentNode->childFiles.begin(), parentNode->childFiles.end(),
		[&](FileNode* node) {
			return node->fileName == fileName;
		});
	return it;
}
SizeInfo FileSystemManager::getNormalizedSize(long long sizeByB) {
	long long newSize = sizeByB;
	int unit = 0;
	while (newSize > 1024 && unit < 3) {
		newSize /= 1024;
		unit++;
	}
	return { newSize, unit };
}
string FileSystemManager::getUnitStr(int unit) {
	switch (unit) {
	case 0:
		return "B";
	case 1:
		return "KB";
	case 2:
		return "MB";
	case 3:
		return "GB";
	default:
		return "UK(Unknown)";
	}
}
void FileSystemManager::showFileInfos(FileNode* fileNode) {
	string fileInfoTemplate = "ID | Dosya/Klasor adi | Klasor/Dosya | Boyut | Olusturulma Tarihi | Son Duzenlenme Tarihi";
	cout << fileInfoTemplate << endl;
	string type,unitStr, fullSizeStr;
	SizeInfo si = getNormalizedSize(fileNode->fileInode.fileSize);
	unitStr = getUnitStr(si.unit);
	fullSizeStr = std::to_string(si.value) + unitStr;
	if (fileNode->isFolder) type = "Klasor";
	else type = "Dosya";
	string fileInfoStr = std::to_string(fileNode->fileInode.fileID) + " | " + fileNode->fileName + " | " + type + " | " + fullSizeStr + " | " + fileNode->fileInode.createdTimeStr + " | " + fileNode->fileInode.lastModifiedTimeStr;
	cout << fileInfoStr << endl;
}
void FileSystemManager::createFile(const string& fileName) {
	fileCount++;
	std::string timeString = calculateFullTime();
	long long fileSizeByB = generateSizeByExtension(fileName);
	Inode newInode = {
		1000 + fileCount,
		fileSizeByB,
		timeString,
		timeString
	};
	FileNode* newFileNode = new FileNode{
		newInode,
		fileName,
		false,
		{},
		current
	};
	current->childFiles.push_back(newFileNode);
	updateParentSizes(newFileNode->parent, fileSizeByB);
}
void FileSystemManager::deleteNode(FileNode* node) {
	for (auto child : node->childFiles) {
		deleteNode(child);
	}
	delete node;
}
void FileSystemManager::deleteFile(FileNode* parentFile, const string& fileName) {
	auto it = findFileIterator(parentFile, fileName);
	if (it == parentFile->childFiles.end()) {
		cout << "Dosya bulunamadi\n";
		return;
	}
	FileNode* fileToDelete = *it;
	parentFile->childFiles.erase(it);
	deleteNode(fileToDelete);
}
void FileSystemManager::createFolder(const string& folderName) {
	fileCount++;
	std::string timeString = calculateFullTime();
	Inode newInode = {
		1000 + fileCount,
		0, // Klasorlerin ilk bastaki boyutu 0. eleman ekledikce boyutu artacak.
		timeString,
		timeString
	};
	FileNode* newFileNode = new FileNode{
		newInode,
		folderName,
		true,
		{},
		current
	};
	current->childFiles.push_back(newFileNode);
}
void FileSystemManager::renameFile(const string& fileName, const string& newName) {
	for (auto fileNode : current->childFiles) {
		if (fileNode->fileName == fileName) {
			fileNode->fileName = newName;
			fileNode->fileInode.lastModifiedTimeStr = calculateFullTime();
			cout << fileName << " dosyasinin ismi '" << newName << "' olarak degistirildi.\n";
		}
	}
}
void FileSystemManager::list() { // No sorting
	cout << "-------------------\n";
	printFiles(current->childFiles);
	cout << "-------------------\n";
}
void FileSystemManager::listBySize() { // list directorys first then files
	vector<FileNode*> childsCopy = current->childFiles;
	sortBySize(childsCopy);
	cout << "-------------------\n";
	printFiles(childsCopy);
	cout << "-------------------\n";
}
void FileSystemManager::listByName() { // list directorys first then files
	vector<FileNode*> childsCopy = current->childFiles;
	sortByName(childsCopy);
	cout << "-------------------\n";
	printFiles(childsCopy);
	cout << "-------------------\n";
}
void FileSystemManager::cd(const string& name) {
	bool found = false;
	for (auto fileNode : current->childFiles) {
		if (fileNode->fileName == name && fileNode->isFolder) {
			found = true;
			current = fileNode;
		}
	}
	if (!found) {
		cout << "Hata: gitmek istenilen klasor (" << name << ") bulunamadi veya bir dosyaya gitmeye calistiniz.\n";
	}
}
void FileSystemManager::cdUp() {
	if (current->parent) {
		current = current->parent;
		return;
	}
	else {
		cout << "Hata: Bir ust klasore gidilemez. En ust klasordesiniz.\n";
	}
}
void FileSystemManager::printCurrentPath() {
	FileNode* temp = current;
	string pathStr;
	while (temp) {
		pathStr = ">>" + temp->fileName + pathStr;
		temp = temp->parent;
	}
	cout << pathStr << ">>";
}
void FileSystemManager::printCommands() {
	cout << "---------\n";
	cout << "***Komutlar***\n";
	cout << "help -> Komutlari gösterir\n";
	cout << "exit -> Programdan cikar\n";
	cout << "cfi [argument]-> Dosya olusturur\n";
	cout << "cfo [argument] -> Klasor olusturur\n";
	cout << "finfo [argument] -> Dosya/Klasor bilgilerini gosterir\n";
	cout << "df [argument] -> Dosya/Klasor siler\n";
	cout << "ref [argument] -> Dosya/Klasor yeniden adlandirir\n";
	cout << "cl [argument] -> Konum degistirir (argument '..' girilirse bir ust klasore cikar.)\n";
	cout << "ls -size -> Mevcut konumdaki dosya ve klasorleri artan boyuta gore listeler\n";
	cout << "ls -name -> Mevcut konumdaki dosya ve klasorleri alfabetik siraya gore listeler\n";
	cout << "---------\n";
}
FileNode* FileSystemManager::getCurrent() { return current; }

