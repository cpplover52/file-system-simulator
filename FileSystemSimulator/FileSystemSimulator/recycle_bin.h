#pragma once
#include "my_queue.h"       
//#include "file_system.h" 
#include "time_utils.h"
struct FileNode; // Ön bildiirm.
struct RecycleItem {
    FileNode* deletedFile;
    FullTime deleteTime;
};

class RecycleBin {
private:
    MyQueue<RecycleItem*> binQueue; 
    int capacity;
    void printRecursively(Node<RecycleItem*>* currentNode, int& count);

public:
    RecycleBin(int cap = 5); // Varsayýlan kapasite 5
    void addToBin(FileNode* file);
    void listBin();
};