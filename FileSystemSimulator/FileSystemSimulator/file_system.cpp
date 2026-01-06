#include "file_system.h"
#include "time_utils.h"
#include <string>
#include <ctime>
FileSystemManager::FileSystemManager() {
	root = new FileNode{ {0, 0, calculateFullTime()} ,"A:", true, {}, nullptr };
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
		cout << "Dosya bulunamadi.\n";
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
		return 100 + (rand() % 900);         // 100B - 1KB
	}
	else if (std::find(pictureExtensions.begin(), pictureExtensions.end(), ext) != pictureExtensions.end()) {
		return 1024 * (500 + (rand() % 2000)); // 500KB - 2.5MB
	}
	else if (std::find(videoExtensions.begin(), videoExtensions.end(), ext) != videoExtensions.end()) {
		return 1024LL * 1024 * (10 + (rand() % 500)); // 10MB - 500MB
	}
	else if (std::find(audioExtensions.begin(), audioExtensions.end(), ext) != audioExtensions.end()) {
		return 1024LL * 1024 * (1 + (rand() % 10000)); // 1MB - 10MB
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
vector<FileNode*>::iterator FileSystemManager::findFileIterator(FileNode* parentNode, const string& fileName) {
	auto it = find_if(parentNode->childFiles.begin(), parentNode->childFiles.end(),
		[&](FileNode* node) {
			return node->fileName == fileName;
		});
	return it;
}
void FileSystemManager::showFileInfos(FileNode* fileNode) {
	string fileInfoTemplate = "ID | Dosya/Klasor adi | Klasor/Dosya | Boyut | Olusturulma Tarihi | Son Duzenlenme Tarihi";
	cout << fileInfoTemplate << endl;
	string type;
	if (fileNode->isFolder) type = "Klasor";
	else type = "Dosya";
	string fileInfoStr = std::to_string(fileNode->fileInode.fileID) + " | " + fileNode->fileName + " | " + type + " | " + std::to_string(fileNode->fileInode.fileSize) + " | " + fileNode->fileInode.createdTimeStr + " | " + fileNode->fileInode.lastModifiedTimeStr;
	cout << fileInfoStr << endl;
}
void FileSystemManager::createFile(const string& fileName) {
	fileCount++;
	std::string timeString = calculateFullTime();
	long long fileSizeByKB = generateSizeByExtension(fileName);
	Inode newInode = {
		1000 + fileCount,
		fileSizeByKB,
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
void FileSystemManager::list() { // list directorys first then files
	int i = 1;
	cout << "-------------------\n";
	for (auto fileNode : current->childFiles)
	{
		string fileType;
		if (fileNode->isFolder) fileType = "folder";
		else fileType = "file";
		cout << i << ". " << fileNode->fileName << "(" << fileType << ")\n";
		i++;
	}
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
	cout << "df [argument] -> Dosya/Klasor siler\n";
	cout << "ref [argument] -> Dosya/Klasor yeniden adlandirir\n";
	cout << "cl [argument] -> Konum degistirir (argument '..' girilirse bir ust klasore cikar.)\n";
	cout << "ls -size -> Mevcut konumdaki dosya ve klasorleri artan boyuta gore listeler\n";
	cout << "ls -name -> Mevcut konumdaki dosya ve klasorleri alfabetik siraya gore listeler\n";
	cout << "---------\n";
}
FileNode* FileSystemManager::getCurrent() { return current; }

