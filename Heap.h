#ifndef HEAP_H
#define HEAP_H

#include <iostream>
#include <vector>
#include <stdexcept> // for underflow_error
using namespace std;

template <typename T>
class Heap {
private:
    vector<T> heap;

    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (heap[index] > heap[parent]) {
                swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(size_t index) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t largest = index;

        if (left < heap.size() && heap[left] > heap[largest]) largest = left;
        if (right < heap.size() && heap[right] > heap[largest]) largest = right;

        if (largest != index) {
            swap(heap[index], heap[largest]);
            heapifyDown(largest);
        }
    }

public:
    void insert(const T& value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    T extractMax() {
        if (heap.empty()) throw underflow_error("Heap is empty.");
        T maxVal = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return maxVal;
    }

    T findMax() const {
        if (heap.empty()) throw underflow_error("Heap is empty.");
        return heap[0];
    }

    size_t getSize() const {
        return heap.size();
    }

    bool isEmpty() const {
        return heap.empty();
    }

    void clear() {
        heap.clear();
    }

    void buildHeap(const vector<T>& elements) {
        heap = elements;
        for (int i = (heap.size() / 2) - 1; i >= 0; --i) {
            heapifyDown(i);
        }
    }

    vector<T> heapSort() {
        vector<T> sorted;
        while (!heap.empty()) {
            sorted.push_back(extractMax());
        }
        return sorted;
    }

    void display() const {
        for (const auto& val : heap) {
            cout << val << " ";
        }
        cout << endl;
    }
void remove(const T& value) {
    auto it = find(heap.begin(), heap.end(), value);
    if (it == heap.end()) throw invalid_argument("Value not found in heap.");

    size_t index = it - heap.begin();
    heap[index] = heap.back();
    heap.pop_back();

    heapifyDown(index);
}
    string asString() const {
        string result = "[ ";
        for (const auto& val : heap) {
            result += to_string(val) + " ";
        }
        result += "]";
        return result;
    }
};

#endif // HEAP_H