#ifndef CIRCULARLINKEDLIST_H
#define CIRCULARLINKEDLIST_H

#include <iostream>
using namespace std;

template <typename T>
class CircularLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* tail;
    size_t size;

public:
    CircularLinkedList() : tail(nullptr), size(0) {}

    ~CircularLinkedList() {
        while (size > 0) remove();
    }

    void insert(const T& value) {
        Node* newNode = new Node(value);
        if (!tail) {
            tail = newNode;
            tail->next = tail;
        } else {
            newNode->next = tail->next;
            tail->next = newNode;
            tail = newNode;
        }
        ++size;
    }

    void remove() {
        if (!tail) return;
        Node* toDelete = tail->next;
        if (toDelete == tail) {
            tail = nullptr;
        } else {
            tail->next = toDelete->next;
        }
        delete toDelete;
        --size;
    }

    bool contains(const T& value) const {
        if (!tail) return false;
        Node* current = tail->next;
        do {
            if (current->data == value) return true;
            current = current->next;
        } while (current != tail->next);
        return false;
    }

    void display() const {
        if (!tail) {
            cout << "List is empty." << endl;
            return;
        }
        Node* current = tail->next;
        do {
            cout << current->data << " ";
            current = current->next;
        } while (current != tail->next);
        cout << endl;
    }
};

#endif // CIRCULARLINKEDLIST_H

