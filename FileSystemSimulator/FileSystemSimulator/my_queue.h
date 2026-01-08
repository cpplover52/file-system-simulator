#pragma once
#include <iostream>

template <typename T>
struct Node {
	T data;
	Node* next;
};

template <typename T>
class MyQueue {
private:
	Node<T>* frontNode;
	Node<T>* rearNode;
	int count;

public:
	MyQueue() {
		frontNode = nullptr;
		rearNode = nullptr;
		count = 0;
	}
	~MyQueue() {
		while (!isEmpty()) {
			dequeue();
		}
	}

	void enqueue(T value) {
		Node<T>* newNode = new Node<T>;
		newNode->data = value;
		newNode->next = nullptr;
		if (rearNode == nullptr) {
			frontNode = rearNode = newNode;
		}
		else {
			rearNode->next = newNode;
			rearNode = newNode;
		}
		count++;
	}

	T dequeue() {
		if (frontNode == nullptr) {
			return T();
		}
		Node<T>* temp = frontNode;
		T data = temp->data;
		frontNode = frontNode->next;
		if (frontNode == nullptr) {
			rearNode = nullptr;
		}
		delete temp;
		count--;
		return data;
	}

	T front() {
		if (frontNode != nullptr) return frontNode->data;
		return T();
	}
	Node<T>* getHead() { return frontNode; }

	bool isEmpty() { return count == 0; }

	int size() { return count; }
};