#include "file_system.h"
#include "sort_utils.h"
#include "recycle_bin.h"
#include "time_utils.h"
#include "my_queue.h"
#include "my_stack.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <ctime>
#include <math.h>
#include <vector>
using std::vector;
using std::transform;
using std::tolower;
FileSystemManager::FileSystemManager() {
	fileTable.resize(tableSize, nullptr);
	FullTime currentTime = getCurrentTime();
	root = new FileNode{ {0, 0, currentTime, currentTime, true} ,"A:", {}, nullptr };
	current = root;
}
FileSystemManager::~FileSystemManager() {
	deleteFile(root->parent, root->fileName);
}
void FileSystemManager::undo() {
	if (undoStack.isEmpty()) {
		cout << "Geri alinacak islem yok.\n";
		return;
	}

	Command cmd = undoStack.top();
	undoStack.pop();

	if (cmd.type == CMD_MOVE) {
		// Geri Al: Hedef klasörden (targetNode) al, eski klasöre (parentNode) geri koy
		// Not: Burada dosyayý "fileName" ile hedef klasörde bulmalýyýz.
		FileNode* movedFile = findFile(cmd.targetNode, cmd.fileName);

		if (movedFile != nullptr) {
			moveFileNode(movedFile, cmd.parentNode); // TERSÝNE ÝÞLEM
			cout << "Undo: Tasima geri alindi.\n";
			redoStack.push(cmd);
		}
	}
	else if (cmd.type == CMD_CREATE) {
		// Oluþturmayý geri al -> Dosyayý SÝL
		// Not: deleteFile fonksiyonunu kullanýyoruz
		deleteFile(cmd.parentNode, cmd.fileName);
		cout << "Undo: " << cmd.fileName << " silindi.\n";

		// Redo için stack'e at
		redoStack.push(cmd);
	}
	else if (cmd.type == CMD_RENAME) {
		// Ýsim deðiþtirmeyi geri al -> Eski isme (oldName) dön
		// Manuel rename iþlemi yapýyoruz (fonksiyonu çaðýrmýyoruz ki tekrar stack'e girmesin)
		auto it = findFileIterator(cmd.parentNode, cmd.fileName);
		if (it != cmd.parentNode->childFiles.end()) {
			(*it)->fileName = cmd.oldName;
			cout << "Undo: Isim " << cmd.oldName << " olarak geri alindi.\n";
		}
		redoStack.push(cmd);
	}
}
void FileSystemManager::redo() {
	if (redoStack.isEmpty()) {
		cout << "Ileri alinacak islem yok.\n";
		return;
	}

	Command cmd = redoStack.top();
	redoStack.pop();
	if (cmd.type == CMD_MOVE) {
		// Ýleri Al: Eski klasörden (parentNode) al, hedef klasöre (targetNode) tekrar koy
		FileNode* fileToMove = findFile(cmd.parentNode, cmd.fileName);

		if (fileToMove != nullptr) {
			moveFileNode(fileToMove, cmd.targetNode); // TEKRAR YAP
			cout << "Redo: Tasima tekrar yapildi.\n";
			undoStack.push(cmd);
		}
	}
	else if (cmd.type == CMD_CREATE) {
		cout << "Redo: " << cmd.fileName << " tekrar olusturuluyor...\n";
		createFile(cmd.fileName, true);
		undoStack.pop(); 
		undoStack.push(cmd); 
	}
	else if (cmd.type == CMD_RENAME) {
		auto it = findFileIterator(cmd.parentNode, cmd.oldName);
		if (it != cmd.parentNode->childFiles.end()) {
			(*it)->fileName = cmd.fileName;
			cout << "Redo: Isim tekrar " << cmd.fileName << " yapildi.\n";
		}
		undoStack.push(cmd);
	}
}
vector<string> FileSystemManager::getSuggestionsPublic(string prefix) {
	return autoCompleteTrie.getSuggestions(prefix);
}
void FileSystemManager::suggest(const string& prefix) {
	vector<string> results = autoCompleteTrie.getSuggestions(prefix);
	if (results.empty()) {
		cout << "Oneri bulunamadi.\n";
	}
	else {
		cout << "Oneriler (" << prefix << "...): ";
		for (const string& s : results) {
			cout << s << ", ";
		}
		cout << endl;
	}
}
void FileSystemManager::moveFile(const string& sourceName, const string& destName) {
	// A) Kaynak Dosyayý Bul
	FileNode* sourceNode = findFile(current, sourceName);
	if (sourceNode == nullptr) {
		cout << "Hata: Tasinacak dosya (" << sourceName << ") bulunamadi.\n";
		return;
	}

	// B) Hedef Klasörü Bul
	FileNode* destFolder = nullptr;

	if (destName == "..") {
		// Üst klasöre taþýma
		if (current->parent == nullptr) {
			cout << "Hata: Zaten en ust dizindesiniz, yukari tasinamaz.\n";
			return;
		}
		destFolder = current->parent;
	}
	else {
		// Mevcut konumdaki bir klasöre taþýma
		destFolder = findFile(current, destName);
		if (destFolder == nullptr || !destFolder->fileInode.isFolder) {
			cout << "Hata: Hedef klasor (" << destName << ") bulunamadi veya bu bir dosya.\n";
			return;
		}
	}

	// C) Ýsim Çakýþmasý Kontrolü (Hedefte ayný isimde dosya var mý?)
	FileNode* collision = findFile(destFolder, sourceName);
	if (collision != nullptr) {
		cout << "Hata: Hedef klasorde bu isimde bir dosya zaten var.\n";
		return;
	}

	// D) Asýl Taþýma Ýþlemini Çaðýr
	moveFileNode(sourceNode, destFolder);
	cout << sourceName << " basariyla " << destFolder->fileName << " konumuna tasindi.\n";

	// E) Undo Stack'e Kaydet
	Command cmd;
	cmd.type = CMD_MOVE;
	cmd.fileName = sourceName;
	cmd.parentNode = current;    // eski parent
	cmd.targetNode = destFolder; // yeni parent

	undoStack.push(cmd);
	// Redo stack'i temizlemeyi unutma (MyStack kullanýyorsan while pop yap)
	// while(!redoStack.isEmpty()) redoStack.pop(); 
}
void FileSystemManager::moveFileNode(FileNode* sourceNode, FileNode* newParent) {
	// 1. Eski babasýnýn listesinden çýkar
	FileNode* oldParent = sourceNode->parent;
	auto it = findFileIterator(oldParent, sourceNode->fileName);
	if (it != oldParent->childFiles.end()) {
		oldParent->childFiles.erase(it);
	}

	// 2. Boyutlarý güncelle (Eski babadan boyutu düþ)
	updateParentSizes(oldParent, -(sourceNode->fileInode.fileSize));

	// 3. Yeni babaya ekle
	newParent->childFiles.push_back(sourceNode);
	sourceNode->parent = newParent; // Dosyanýn baba pointer'ýný güncelle

	// 4. Boyutlarý güncelle (Yeni babaya boyutu ekle)
	updateParentSizes(newParent, sourceNode->fileInode.fileSize);
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
FileNode* FileSystemManager::findFile(int fileID) {
	int index = fileID % tableSize;
	FileNode* temp = fileTable[index];
	while (temp != nullptr) {
		if (temp->fileInode.fileID == fileID) {
			return temp;
		}
		temp = temp->nextTableNode;
	}
	return nullptr;
}
void FileSystemManager::listRecycleBin() {
	recycleBin.listBin();
}
void FileSystemManager::searchFileLinear(const string& fileName) {
	FileNode* fileNode = findFile(getCurrent(), fileName);
	if (fileNode == nullptr) {
		cout << "Dosya bulunamadi...";
		return;
	}
	string outputStr = "Dosya bulundu: " + getPath(fileNode) + "\n";
	cout << outputStr;
}
void FileSystemManager::searchFileBinary(const string& fileName) {
	if (current->childFiles.empty()) {
		cout << "Klasor bos, arama yapilamaz.\n";
		return;
	}

	// 1. Sýralama (Küçük harf duyarlý sýralama kullandýðýn için copy de öyle olmalý)
	vector<FileNode*> copyChilds = current->childFiles;
	sortByName(copyChilds);

	int left = 0;
	int right = copyChilds.size() - 1;
	string target = fileName; 
	std::transform(target.begin(), target.end(), target.begin(), ::tolower);// Aranan ismi küçült
	while (left <= right) {
		int mid = left + (right - left) / 2;
		string midName = copyChilds[mid]->fileName; 
		std::transform(midName.begin(), midName.end(), midName.begin(), ::tolower);// Mevcut ismi küçült

		if (midName == target) {
			cout << "Dosya bulundu: " << getPath(copyChilds[mid]) << "\n";
			return;
		}

		if (midName < target) {
			left = mid + 1;
		}
		else {
			right = mid - 1;
		}
	}
	cout << "Dosya mevcut klasorde bulunamadi (Binary).\n";
}
void FileSystemManager::searchFileBFS(const string& fileName) {
	if (root == nullptr) {
		return;
	}

	MyQueue<FileNode*> q;
	q.enqueue(root);
	FileNode* fileNode;
	while (!q.isEmpty()) {
		FileNode* currentItem = q.dequeue(); 
		if (currentItem->fileName == fileName) {
			fileNode = currentItem;
			string outputStr = "Dosya bulundu: " + getPath(fileNode);
			cout << outputStr << endl;
			return;
		}
		if (currentItem->fileInode.isFolder) {
			for (FileNode* child : currentItem->childFiles) {
				q.enqueue(child); 
			}
		}
	}
	cout << "Dosya bulunamadi...";
	return;
}
void FileSystemManager::addToTable(FileNode* fileNode) {
	int index = fileNode->fileInode.fileID % tableSize;
	if (fileTable[index] == nullptr) { // Eðer bos ise
		fileTable[index] = fileNode;
	}
	else { // Eger dolu ise
		FileNode* temp = fileTable[index];
		while (temp->nextTableNode != nullptr) {
			temp = temp->nextTableNode;
		}
		temp->nextTableNode = fileNode;
	}
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
		if (file->fileInode.isFolder) fileType = "folder";
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
//FileNode* FileSystemManager::searchFile(const string& fileName) {
//	// Binary search
//	// Dosyalar isimlerine gore listelenelecek.
//	// Listede binary search yapilarak dosya ismi bulunacak.
//}
void FileSystemManager::listTreeRecursive(FileNode* node, int depth) {
	for (int i = 0; i < depth; i++) {
		cout << "   ";
	}
	cout << "-" << node->fileName;
	if (node->fileInode.isFolder) {
	    cout << "(fol)";
	}
	cout << endl;
	if (node->childFiles.size() > 0) {
		for (auto child : node->childFiles) {
			listTreeRecursive(child, depth+1);
		}
	}
}
void FileSystemManager::showFileInfos(FileNode* fileNode) {
	string fileInfoTemplate = "ID | Dosya/Klasor adi | Klasor/Dosya | Boyut | Olusturulma Tarihi | Son Duzenlenme Tarihi";
	cout << fileInfoTemplate << endl;
	string type,unitStr, fullSizeStr;
	SizeInfo si = getNormalizedSize(fileNode->fileInode.fileSize);
	unitStr = getUnitStr(si.unit);
	fullSizeStr = std::to_string(si.value) + unitStr;
	if (fileNode->fileInode.isFolder) type = "Klasor";
	else type = "Dosya";
	string fileInfoStr = std::to_string(fileNode->fileInode.fileID) + " | " + fileNode->fileName + " | " + type + " | " + fullSizeStr + " | " + fileNode->fileInode.createTime.getFullTimeStr() + " | " + fileNode->fileInode.lastModifyTime.getFullTimeStr();
	cout << fileInfoStr << endl;
}
void FileSystemManager::createFile(const string& fileName, bool isRedoOperation) {
	FileNode* temp = findFile(getCurrent(), fileName);
	if (temp != nullptr && !temp->fileInode.isFolder) {
		cout << "Mevcut konumda ayný isimle bir dosya var." << endl;
		return;
	}
	long long fileSizeByB = generateSizeByExtension(fileName);
	int fileID = 1000 + fileCount;
	FullTime currentTime = getCurrentTime();
	Inode newInode = {
		fileID,
		fileSizeByB,
		currentTime,
		currentTime,
		false
	};
	FileNode* newFileNode = new FileNode{
		newInode,
		fileName,
		{},
		current
	};
	current->childFiles.push_back(newFileNode);
	addToTable(newFileNode);
	autoCompleteTrie.insert(fileName); // Otomatik tamamlama için trie'ye eklendi.
	fileCount++;
	updateParentSizes(newFileNode->parent, fileSizeByB);
	
	if (!isRedoOperation) {
		Command cmd;
		cmd.type = CMD_CREATE;
		cmd.fileName = fileName;
		cmd.parentNode = current;
		undoStack.push(cmd);

		// Redo stack temizliðini sadece YENÝ iþlemse yap
		while (!redoStack.isEmpty()) redoStack.pop();
	}
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
	recycleBin.addToBin(fileToDelete);
	/*deleteNode(fileToDelete);*/
}
void FileSystemManager::createFolder(const string& folderName) {
	FileNode* temp = findFile(getCurrent(), folderName);
	if (temp != nullptr && temp->fileInode.isFolder) {
		cout << "Mevcut konumda ayný isimle bir klasor var." << endl;
		return;
	}
	int fileID = 1000 + fileCount;
	FullTime currentTime = getCurrentTime();
	Inode newInode = {
		fileID,
		0, // Klasorlerin ilk bastaki boyutu 0. eleman ekledikce boyutu dinamik olarak artýcak.
		currentTime,
		currentTime,
		true
	};
	FileNode* newFileNode = new FileNode{
		newInode,
		folderName,
		{},
		current
	};
	current->childFiles.push_back(newFileNode);
	addToTable(newFileNode);
	fileCount++;
}
void FileSystemManager::renameFile(const string& fileName, const string& newName) {
	FileNode* file = findFile(current, fileName);
	if(file == nullptr){
		return;
	}
	file->fileName = newName;
	file->fileInode.lastModifyTime = getCurrentTime();
	cout << fileName << " dosyasinin ismi '" << newName << "' olarak degistirildi.\n";
	// --- UNDO ICIN KAYIT ---
	Command cmd;
	cmd.type = CMD_CREATE;
	cmd.fileName = fileName;
	cmd.parentNode = current;
	undoStack.push(cmd);

	// Yeni islem yapildiginda Redo stack temizlenmeli
	while (!redoStack.isEmpty()) redoStack.pop();
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
void FileSystemManager::listAsTree() {
	/*
	-Oyunlar
		-cs2.exe
		-skyrim.exe
	-Müzikler
		-Lady Gaga
			-Best Ones
				-best_lady_gaga_song.mp3
			-die_with_a_smile.mp3
		music.mp3
	-Videolar
		-Ekran Kayitlari
			-ekran_kaydi_1.mp4
		video.mp4
	*/
	cout << "-------Agac Gosterimi-------\n";
	listTreeRecursive(current, 0);
	cout << "----------------------------\n";
}
void FileSystemManager::cd(const string& name) {
	bool found = false;
	for (auto fileNode : current->childFiles) {
		if (fileNode->fileName == name && fileNode->fileInode.isFolder) {
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
string FileSystemManager::getPath(FileNode* fileNode) {
	if (fileNode == nullptr) return "";
	FileNode* temp = fileNode;
	string pathStr = "";
	while (temp != nullptr) {
		if (temp->parent != nullptr) {
			pathStr = ">>" + temp->fileName + pathStr;
		}
		else {
			pathStr = temp->fileName + pathStr;
		}
		temp = temp->parent;
	}
	return pathStr;
}

void FileSystemManager::printCommands() {
	cout << "---------\n";
	cout << "***Komutlar***\n";
	cout << "help -> Komutlari gösterir\n";
	cout << "undo -> Son islemi geri alir.\n";
	cout << "redo -> Son geri alinan islemi bir daha yapar.\n";
	cout << "cfi [argument]-> Dosya olusturur\n";
	cout << "cfo [argument] -> Klasor olusturur\n";
	cout << "finfo [argument] -> Dosya/Klasor bilgilerini gosterir\n";
	cout << "df [argument] -> Dosya/Klasor siler\n";
	cout << "ref [argument] -> Dosya/Klasor yeniden adlandirir\n";
	cout << "search -bfs [argument]-> BFS ile dosyayi mevcut klasorde ve alt klasorlerde arar.\n";
	cout << "search -bs [argument]-> Binary Search ile dosyayi mevcut klasorde arar.\n";
	cout << "search -ls [argument]-> Linear Search ile dosyayi mevcut klasorde arar.\n";
	cout << "cl [argument] -> Konum degistirir (argument '..' girilirse bir ust klasore cikar.)\n";
	cout << "lsr -> Geri donusum kutusunu listeler.\n";
	cout << "ls -> Olusturulma sirasina gore dosya ve klasorleri listeler.\n";
	cout << "ls -size -> Mevcut konumdaki dosya ve klasorleri dosya boyutuna gore listeler\n";
	cout << "ls -name -> Mevcut konumdaki dosya ve klasorleri alfabetik siraya gore listeler\n";
	cout << "ls -tree -> Mevcut konumun altindaki tum dosya ve klasorleri tree gorunumunde listeler.\n";
	cout << "exit -> Programdan cikar\n";
	cout << "---------\n";
}
FileNode* FileSystemManager::getCurrent() { return current; }
SizeInfo getNormalizedSize(long long sizeByB) {
	long long newSize = sizeByB;
	int unit = 0;
	while (newSize > 1024 && unit < 3) {
		newSize /= 1024;
		unit++;
	}
	return { newSize, unit };
}
string getUnitStr(int unit) {
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