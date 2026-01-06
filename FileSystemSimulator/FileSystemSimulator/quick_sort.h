#pragma once
#include "file_system.h"
#include <vector>

using std::vector;
bool compareBySize(FileNode* a, FileNode* b);
void mySwap(vector<FileNode*>& vec, int i, int j);
int partition(vector<FileNode*>& vec, int low, int high);
void quickSortBigToSmall(vector<FileNode*>& vec, int low, int high);