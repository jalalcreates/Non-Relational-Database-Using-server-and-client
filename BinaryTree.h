#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <functional>
using namespace std;

template <typename T>
class BinaryTree {
private:
    struct Node {
        T data;
        Node* left;
        Node* right;
        Node(const T& val) : data(val), left(nullptr), right(nullptr) {}
    };

    Node* root;

    void insert(Node*& node, const T& value) {
        if (!node) {
            node = new Node(value);
        } else if (value < node->data) {
            insert(node->left, value);
        } else {
            insert(node->right, value);
        }
    }

    void inorder(Node* node, ostringstream& oss) const {
        if (!node) return;
        inorder(node->left, oss);
        oss << node->data << " ";
        inorder(node->right, oss);
    }

    Node* search(Node* node, const T& value) const {
        if (!node || node->data == value) return node;
        if (value < node->data) return search(node->left, value);
        return search(node->right, value);
    }

    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    T findMax(Node* node) const {
        if (!node) throw runtime_error("Tree is empty.");
        while (node->right) {
            node = node->right;
        }
        return node->data;
    }

    T findMin(Node* node) const {
        if (!node) throw runtime_error("Tree is empty.");
        while (node->left) {
            node = node->left;
        }
        return node->data;
    }

    Node* remove(Node* node, const T& value) {
        if (!node) return nullptr;

        if (value < node->data) {
            node->left = remove(node->left, value);
        } else if (value > node->data) {
            node->right = remove(node->right, value);
        } else {
            // Node with only one child or no child
            if (!node->left) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                Node* temp = node->left;
                delete node;
                return temp;
            }

            // Node with two children: Get the inorder successor (smallest in the right subtree)
            Node* temp = findMinNode(node->right);
            node->data = temp->data;
            node->right = remove(node->right, temp->data);
        }
        return node;
    }

    Node* findMinNode(Node* node) const {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }

    int getHeight(Node* node) const {
        if (!node) return 0;
        return 1 + max(getHeight(node->left), getHeight(node->right));
    }

    bool isBalanced(Node* node) const {
        if (!node) return true;
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        return abs(leftHeight - rightHeight) <= 1 && isBalanced(node->left) && isBalanced(node->right);
    }

public:
    BinaryTree() : root(nullptr) {}

    ~BinaryTree() { clear(root); }

    void insert(const T& value) { insert(root, value); }

    bool search(const T& value) const { return search(root, value) != nullptr; }

    void displayInOrder() const {
        ostringstream oss;
        inorder(root, oss);
        cout << oss.str() << endl;
    }

    string inorderAsString() const {
        ostringstream oss;
        inorder(root, oss);
        return oss.str();
    }

    T findMax() const { return findMax(root); }

    T findMin() const { return findMin(root); }

    size_t size() const {
        size_t count = 0;
        function<void(Node*)> countNodes = [&](Node* node) {
            if (!node) return;
            ++count;
            countNodes(node->left);
            countNodes(node->right);
        };
        countNodes(root);
        return count;
    }

    bool isEmpty() const { return root == nullptr; }

    void remove(const T& value) {
        root = remove(root, value);
    }

    int getHeight() const {
        return getHeight(root);
    }

    bool isBalanced() const {
        return isBalanced(root);
    }

    void clear() {
        clear(root);
        root = nullptr;
    }
};

#endif // BINARYTREE_H