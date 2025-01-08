#ifndef CIRCULARLINKEDLIST_H
#define CIRCULARLINKEDLIST_H

#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

template <typename T>
class CircularLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* tail;  // Pointer to the tail of the list
    size_t size; // Number of elements in the list

public:
    // Constructor
    CircularLinkedList() : tail(nullptr), size(0) {}

    // Destructor
    ~CircularLinkedList() {
        clear();
    }

    // Insert a value at the end of the list
    void insert(const T& value) {
        Node* newNode = new Node(value);
        if (!tail) {
            tail = newNode;
            tail->next = tail; // Circular reference
        } else {
            newNode->next = tail->next; // New node points to the head
            tail->next = newNode;       // Tail points to the new node
            tail = newNode;             // Update tail to the new node
        }
        ++size;
    }

    // Remove the first node and return its value
    T remove() {
        if (size == 0) throw runtime_error("List is empty.");

        Node* toDelete = tail->next; // The node to delete (head)
        T data = toDelete->data;     // Save the data to return

        if (size == 1) {
            tail = nullptr; // Only one node, set tail to null
        } else {
            tail->next = toDelete->next; // Bypass the node to delete
        }

        delete toDelete;
        --size;
        return data; // Return the removed data
    }

    // Check if the list contains a value
    bool contains(const T& value) const {
        if (size == 0) return false;

        Node* current = tail->next; // Start from the head
        do {
            if (current->data == value) return true;
            current = current->next;
        } while (current != tail->next); // Traverse the circular list

        return false;
    }

    // Clear the list
    void clear() {
        while (size > 0) {
            remove(); // Remove nodes one by one
        }
    }

    // Convert the list to a string representation
    string asString() const {
        if (size == 0) return "";

        ostringstream oss;
        Node* current = tail->next; // Start from the head
        do {
            oss << current->data << " ";
            current = current->next;
        } while (current != tail->next); // Traverse the circular list

        string result = oss.str();
        if (!result.empty()) {
            result.pop_back(); // Remove the trailing space
        }
        return result;
    }

    // Display the list
    void display() const {
        if (size == 0) {
            cout << "List is empty." << endl;
            return;
        }

        Node* current = tail->next; // Start from the head
        do {
            cout << current->data << " ";
            current = current->next;
        } while (current != tail->next); // Traverse the circular list
        cout << endl;
    }

    // Get the size of the list
    size_t getSize() const {
        return size;
    }

    // Check if the list is empty
    bool isEmpty() const {
        return size == 0;
    }
};

#endif // CIRCULARLINKEDLIST_H