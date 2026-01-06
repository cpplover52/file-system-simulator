#include "sort_utils.h"
#include "quick_sort.h"
#include "merge_sort.h"
#include "file_system.h"
#include <vector>
using std::vector;
void sortBySize(vector<FileNode*>& files) {
	// Quick Sort
	quickSortBigToSmall(files, 0, files.size()-1);
}
void sortByName(vector<FileNode*>& files) {
	// Merge Sort
	mergeSort(files, 0, files.size()-1);
}