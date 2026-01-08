#pragma once
#include "time_utils.h"
#include "recycle_bin.h"
#include "my_stack.h"
#include "trie.h"
#include <iostream>
#include <unordered_map>
#include <ctime>
#include <vector>
using namespace std;
enum CommandType { CMD_CREATE, CMD_RENAME, CMD_MOVE };

struct Command {
	CommandType type;
	string fileName;     // Ýþlem yapýlan dosya
	string oldName;      // Rename iþlemi geri almak için eski isim
	FileNode* parentNode; // eski parent
	FileNode* targetNode; // ynei parent
};
struct SizeInfo {
	long long value;
	int unit; // 0=B, 1=KB, 2=MB, 3=GB
};
struct Inode {
	int fileID;
	long long fileSize; // Byte
	FullTime createTime;
	FullTime lastModifyTime;
	bool isFolder;
};
struct FileNode {
	Inode fileInode;
	string fileName;
	vector<FileNode*> childFiles;
	FileNode* parent;
	FileNode* nextTableNode;
};

class FileSystemManager {
private:
	FileNode* root;
	FileNode* current;
	int fileIDOffset = 1000;
	int fileCount = 0;
	int tableSize = 10;
	vector<FileNode*> fileTable;
	vector<string> textExtensions = { "txt","json", "html", "css", "js", "log", "c", "cs", "cpp", "java", "py" };
	vector<string> pictureExtensions = { "jpg","jpeg","png","svg" };
	vector<string> videoExtensions = { "mov","mp4","mkv", "avi" };
	vector<string> audioExtensions = { "mp3", "wav" };
	MyStack<Command> undoStack;    
	MyStack<Command> redoStack;    
	Trie autoCompleteTrie;
	long long generateSizeByExtension(const string& fileName);
	void updateParentSizes(FileNode* node, long long sizeChange);
	RecycleBin recycleBin;
	void printFiles(vector<FileNode*> files);
	void addToTable(FileNode* fileNode);
	void listTreeRecursive(FileNode* node, int depth);
public: 
	FileSystemManager();
	~FileSystemManager();
	void undo();
	void redo();
	vector<string> getSuggestionsPublic(string prefix);
	void suggest(const string& prefix);
	FileNode* findFile(FileNode* fileNode,const string& fileName);
	FileNode* findFile(int fileID);
	vector<FileNode*>::iterator findFileIterator(FileNode* fileNode, const string& fileName);
	void listRecycleBin();
	void showFileInfos(FileNode* fileNode);
	void createFile(const string& fileName, bool isRedoOperation = false);
	void createFolder(const string& folderName);
	void moveFile(const string& sourceName, const string& destName);
	void moveFileNode(FileNode* sourceNode, FileNode* newParent);
	void searchFileBFS(const string& fileName);
	void searchFileLinear(const string& fileName);
	void searchFileBinary(const string& fileName);
	void deleteNode(FileNode* node);
	void deleteFile(FileNode* parentFile,const string& fileName);
	void renameFile(const string& fileName, const string& newName);
	void list();
	void listBySize();
	void listByName();
	void listAsTree();
	void cd(const string& name);
	void cdUp();
	string getPath(FileNode* fileNode);
	void printCommands();
	FileNode* getCurrent();
};
SizeInfo getNormalizedSize(long long size);
string getUnitStr(int unit);