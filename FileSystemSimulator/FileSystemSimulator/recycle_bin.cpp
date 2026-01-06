#include "recycle_bin.h"
void RecycleBin::enqueue(FileNode* fileNode) {
	sizeCount++;
}
void RecycleBin::dequeue() {
	sizeCount--;
}
int RecycleBin::size() {
	return sizeCount;
}
bool RecycleBin::isEmpty() {
	return sizeCount == 0;
}