
#ifndef LIST_H_
#define LIST_H_

#include "wrappers.h"

class PCB;
typedef int ID;

template<class T>
class Node;


template<class T>
class List {
public:
	Node<T>* head, *tail;
	volatile ID currId;

	List() : head(0), tail(0),  currId(1){}

	~List() {
		while (head != 0) {
		Node<T>* t = head;
		head = head->next;
		dealloc(t);
		}
	}

	T* find(ID Id);
	Node<T>* find(T* i);
	T* remove(ID Id);
	T* remove(T* i);

	ID frontId() {return head ? head->Id : 0;}

	int empty() {if (head == 0) return 1; else return 0;}
	T* peekFront();
	T* peekBack();
	T* popFront();
	T* popBack();
	int pushFront(T* i);
	int pushBack(T* i);

	void insertByTime(T*);


};

template<class T>
class Node {
public:
	T* info;
	ID Id;
	Node* next, *prev;
	Node(T* i, Node* prev, Node* next, ID currId) : info(i), prev(prev), next(next), Id(currId){
		if (prev) prev->next = this;
		if (next) next->prev = this;
	}
	~Node(){
		if (prev) prev->next = this->next;
		if (next) next->prev = this->prev;
		}
};



template<class T>
T* List<T>::find(ID Id) {
	if (head == 0) return 0;
	Node<T>* t = head;
	while (t != 0 && t->Id != Id) t = t->next;
	if (t != 0) return t->info;
	else return 0;
}

template<class T>
Node<T>* List<T>::find(T* i){
	if (head == 0) return 0;
	Node<T>* t = head;
	while (t != 0 && t->info != i) t = t->next;
	return t;
}

template<class T>
T* List<T>::remove(T* i) {
	if (head == 0 || i == 0) return 0;
	Node<T>* t = find(i);
	if (t == 0) return 0;
	if (t == tail) tail = tail->prev;
	if (t == head) head = head->next;
	T* ret = t->info;
	dealloc(t);
	return ret;
}

template<class T>
T* List<T>::remove(ID Id) {
	return remove(find(Id));
}

template<class T>
T* List<T>::peekFront() { if (head) return head->info; else return 0;}

template<class T>
T* List<T>::peekBack() { if (head) return tail->info; else return 0;}

template<class T>
T* List<T>::popFront() {
	if (head == 0) return 0;
	Node<T>* t = head;
	if (head == tail) tail = tail->prev;
	head = head->next;
	T* ret = t->info;
	dealloc(t);
	return ret;
}

template<class T>
T* List<T>::popBack() {
	if (head == 0) return 0;
	Node<T>* t = tail;
	tail = tail->prev;
	if (t == head) head = head->next;
	T* ret = t->info;
	dealloc(t);
	return ret;
}

template<class T>
int List<T>::pushFront(T* i){
	alloc(head, Node<T>(i, 0, head, currId));
	if (!tail) tail = head;
	return currId++;
}

template<class T>
int List<T>::pushBack(T* i){
	alloc(tail, Node<T>(i, tail, 0, currId));
	if (!head) head = tail;
	return currId++;
}

#endif /* LIST_H_ */
