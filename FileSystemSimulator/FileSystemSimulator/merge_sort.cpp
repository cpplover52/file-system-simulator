#include "file_system.h"
#include "merge_sort.h"
#include <vector>
#include <algorithm>
#include <cctype>
using std::vector;
using std::transform;
using std::tolower;
void merge(vector<FileNode*>& arr, int left,
    int mid, int right) {

    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<FileNode*> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        string lName = L[i]->fileName;  
        string rName = R[j]->fileName;  
        std::transform(lName.begin(), lName.end(), lName.begin(), ::tolower);
        std::transform(rName.begin(), rName.end(), rName.begin(), ::tolower);

        if (lName <= rName) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(vector<FileNode*>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}