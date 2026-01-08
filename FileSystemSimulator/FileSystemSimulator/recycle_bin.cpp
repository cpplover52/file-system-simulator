#include "recycle_bin.h"
#include "time_utils.h"
#include "file_system.h"
#include <string>
using std::string;
RecycleBin::RecycleBin(int cap) {
	capacity = cap;
}
void RecycleBin::addToBin(FileNode* fileNode) {
	if (binQueue.size() >= capacity) {
		RecycleItem* oldItem = binQueue.dequeue(); // En eskiyi kuyruktan çýkar
		// FileNode* ve REcycleItem* bellekten kalýcý olarak siliniyor.
		delete oldItem->deletedFile; 
		delete oldItem;
	}
	RecycleItem* ri = new RecycleItem;
	ri->deletedFile = fileNode;
	ri->deleteTime = getCurrentTime();
	binQueue.enqueue(ri);
}
void RecycleBin::listBin() {
	int count = 0;
	//Node<RecycleItem*>* head = ;
	cout << "----------Cop Kutusu----------\n";
	cout << "Dosya ismi | Silme tarihi | dosya boyutu\n";
	printRecursively(binQueue.getHead(), count);
	cout << "------------------------------\n";
}
void RecycleBin::printRecursively(Node<RecycleItem*>* currentNode, int& count) {
	if (currentNode == nullptr) return;
	printRecursively(currentNode->next, count);
	string fileName = currentNode->data->deletedFile->fileName;
	string deleteTimeStr = currentNode->data->deleteTime.getFullTimeStr();
	SizeInfo si = getNormalizedSize(currentNode->data->deletedFile->fileInode.fileSize);
	string valueStr = std::to_string(si.value);
	string unitStr = getUnitStr(si.unit);
	string fullSizeStr = std::to_string(si.value) + unitStr; 
	cout << fileName << " | " << deleteTimeStr << " | " << fullSizeStr << endl;
}
