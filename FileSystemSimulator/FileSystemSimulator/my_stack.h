#pragma once
#include <iostream>
using namespace std;

template <typename T>
struct StackNode {
	T data;
	StackNode* next;
};

template <typename T>
class MyStack {
private:
    StackNode<T>* topNode; 
    int count;
public:
    MyStack() {
        topNode = nullptr;
        count = 0;
    }

    ~MyStack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(T value) {
        StackNode<T>* newNode = new StackNode<T>;
        newNode->data = value;
        newNode->next = topNode; 
        topNode = newNode;       
        count++;
    }

    void pop() {
        if (isEmpty()) return;

        StackNode<T>* temp = topNode;
        topNode = topNode->next; // Bir alt eleman top yapýlýyor.
        delete temp;
        count--;
    }

    T top() {
        if (topNode != nullptr) {
            return topNode->data;
        }
        return T();
    }

    bool isEmpty() { return topNode == nullptr; }
    int size() { return count; }
};