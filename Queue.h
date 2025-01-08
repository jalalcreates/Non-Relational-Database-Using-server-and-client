#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* front;  // Pointer to the front of the queue
    Node* rear;   // Pointer to the rear of the queue
    size_t count; // Number of elements in the queue

public:
    // Constructor
    Queue() : front(nullptr), rear(nullptr), count(0) {}

    // Destructor
    ~Queue() {
        while (!isEmpty()) dequeue();
    }

    // Add an element to the rear of the queue
    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        if (!rear) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        ++count;
    }

    // Remove an element from the front of the queue
    T dequeue() {
        if (isEmpty()) throw underflow_error("Queue is empty.");
        Node* toDelete = front;
        T data = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete toDelete;
        --count;
        return data;
    }

    // Get the element at the front of the queue
    T peek() const {
        if (isEmpty()) throw underflow_error("Queue is empty.");
        return front->data;
    }

    // Check if the queue is empty
    bool isEmpty() const {
        return count == 0;
    }

    // Get the size of the queue
    size_t size() const {
        return count;
    }

    // Clear the queue
    void clear() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    // Display the queue elements
    void display() const {
        Node* current = front;
        while (current) {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }

    // Convert the queue to a string representation
    string asString() const {
        if (isEmpty()) return "[]";
        ostringstream oss;
        oss << "[";
        Node* current = front;
        while (current) {
            oss << current->data;
            if (current->next) oss << ", ";
            current = current->next;
        }
        oss << "]";
        return oss.str();
    }

    // Find an element in the queue
    bool find(const T& value) const {
        Node* current = front;
        while (current) {
            if (current->data == value) return true;
            current = current->next;
        }
        return false;
    }

    // Get the front element of the queue
    T getFront() const {
        if (isEmpty()) throw underflow_error("Queue is empty.");
        return front->data;
    }

    // Get the rear element of the queue
    T getRear() const {
        if (isEmpty()) throw underflow_error("Queue is empty.");
        return rear->data;
    }

    // Access elements using the [] operator
    T operator[](size_t index) const {
        if (index >= count) throw out_of_range("Index out of range.");
        Node* current = front;
        for (size_t i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }

    // Convert the queue to a vector
    vector<T> toVector() const {
        vector<T> result;
        Node* current = front;
        while (current) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }
};

#endif // QUEUE_H