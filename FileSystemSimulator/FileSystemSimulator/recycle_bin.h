#pragma once
#include "file_system.h"
struct RecycleNode {
	FileNode* file;
	FileNode* originalParent;
	RecycleNode* next;
};
class RecycleBin {
private:
	FileNode* rear;
	FileNode* front;
	int sizeCount = 0;
public:
	void enqueue(FileNode* fileNode);
	void dequeue();
	int size();
	bool isEmpty();
};