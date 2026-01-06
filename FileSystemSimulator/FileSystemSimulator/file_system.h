#pragma once
#include <iostream>
#include <ctime>
#include <vector>
using namespace std;
struct Inode {
	int fileID;
	long long fileSize;
	string createdTimeStr;
	string lastModifiedTimeStr;
};
struct FileNode {
	Inode fileInode;
	string fileName;
	bool isFolder;
	vector<FileNode*> childFiles;
	FileNode* parent;
};

class FileSystemManager {
private:
	FileNode* root;
	FileNode* current;
	int fileIDOffset = 1000;
	int fileCount = 0;
	vector<string> textExtensions = { "txt","json", "html", "css", "js", "log", "c", "cs", "cpp", "java", "py" };
	vector<string> pictureExtensions = { "jpg","jpeg","png","svg" };
	vector<string> videoExtensions = { "mov","mp4","mkv", "avi" };
	vector<string> audioExtensions = { "mp3", "wav" };
	long long generateSizeByExtension(const string& fileName);
	void updateParentSizes(FileNode* node, long long sizeChange);
public: 
	FileSystemManager();
	~FileSystemManager();
	FileNode* findFile(FileNode* fileNode,const string& fileName);
	vector<FileNode*>::iterator findFileIterator(FileNode* fileNode, const string& fileName);
	void showFileInfos(FileNode* fileNode);
	void createFile(const string& fileName);
	void createFolder(const string& folderName);
	void deleteNode(FileNode* node);
	void deleteFile(FileNode* parentFile,const string& fileName);
	void renameFile(const string& fileName, const string& newName);
	void list();
	void cd(const string& name);
	void cdUp();
	void printCurrentPath();
	void printCommands();
	FileNode* getCurrent();
};