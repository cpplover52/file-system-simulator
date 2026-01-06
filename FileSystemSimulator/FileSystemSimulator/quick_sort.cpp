#include "quick_sort.h"
#include "file_system.h"
#include <iostream>
#include <vector>
using std::vector;
bool compareBySize(FileNode* a, FileNode* b) {
	// Biri klasör diðeri dosyaysa, klasör (isFolder=true) her zaman küçüktür (üstte yer alýr).
	if (a->isFolder && !b->isFolder) return true;
	if (!a->isFolder && b->isFolder) return false;
	
	return a->fileInode.fileSize >= b->fileInode.fileSize;
}
void mySwap(vector<FileNode*>& vec, int i, int j) {
	FileNode* temp = vec[i];
	vec[i] = vec[j];
	vec[j] = temp;
}
int partition(vector<FileNode*>& vec, int low, int high) {
	FileNode* pivot = vec[high];
	int i = low - 1, j = low;
	for (j; j <= high - 1; j++) {
		if (compareBySize(vec[j], pivot)) {
			i++;
			mySwap(vec, i ,j);
		}
	}
	mySwap(vec, i+1, high); // Setting pivot's location
	return (i + 1);
}

void quickSortBigToSmall(vector<FileNode*>& vec, int low, int high) {
	if (high > low) {
		int pi = partition(vec, low, high);

		quickSortBigToSmall(vec,low,pi-1);
		quickSortBigToSmall(vec, pi+1, high);
	}
}