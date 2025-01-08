#ifndef AVLTREE_H
#define AVLTREE_H

#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace std;

template <typename T>
class AVLTree {
private:
    struct Node {
        T data;
        int height;
        Node* left;
        Node* right;
        Node(const T& val) : data(val), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root;

    int height(Node* node) const { return node ? node->height : 0; }

    int balanceFactor(Node* node) const { return node ? height(node->left) - height(node->right) : 0; }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
    }

    Node* insert(Node* node, const T& value) {
        if (!node) return new Node(value);

        if (value < node->data) {
            node->left = insert(node->left, value);
        } else if (value > node->data) {
            node->right = insert(node->right, value);
        } else {
            return node; // Duplicates not allowed
        }

        node->height = 1 + max(height(node->left), height(node->right));

        int balance = balanceFactor(node);

        if (balance > 1 && value < node->left->data) return rotateRight(node);
        if (balance < -1 && value > node->right->data) return rotateLeft(node);
        if (balance > 1 && value > node->left->data) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && value < node->right->data) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Node* remove(Node* node, const T& value) {
        if (!node) return nullptr;

        if (value < node->data) {
            node->left = remove(node->left, value);
        } else if (value > node->data) {
            node->right = remove(node->right, value);
        } else {
            // Node with only one child or no child
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp; // Copy the contents of the non-empty child
                }
                delete temp;
            } else {
                // Node with two children: Get the inorder successor (smallest in the right subtree)
                Node* temp = findMinNode(node->right);
                node->data = temp->data;
                node->right = remove(node->right, temp->data);
            }
        }

        if (!node) return nullptr;

        // Update height and balance the tree
        node->height = 1 + max(height(node->left), height(node->right));
        int balance = balanceFactor(node);

        // Left Left Case
        if (balance > 1 && balanceFactor(node->left) >= 0) {
            return rotateRight(node);
        }

        // Left Right Case
        if (balance > 1 && balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right Right Case
        if (balance < -1 && balanceFactor(node->right) <= 0) {
            return rotateLeft(node);
        }

        // Right Left Case
        if (balance < -1 && balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Node* findMinNode(Node* node) const {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }

    void inorder(Node* node, ostringstream& oss) const {
        if (!node) return;
        inorder(node->left, oss);
        oss << node->data << " ";
        inorder(node->right, oss);
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

    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    size_t size(Node* node) const {
        if (!node) return 0;
        return 1 + size(node->left) + size(node->right);
    }

    bool isBalanced(Node* node) const {
        if (!node) return true;
        int balance = balanceFactor(node);
        return abs(balance) <= 1 && isBalanced(node->left) && isBalanced(node->right);
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree() { clear(root); }

    void insert(const T& value) { root = insert(root, value); }

    void remove(const T& value) { root = remove(root, value); }

    string inorderAsString() const {
        ostringstream oss;
        inorder(root, oss);
        return oss.str();
    }

    T findMax() const { return findMax(root); }

    T findMin() const { return findMin(root); }

    size_t size() const { return size(root); }

    bool isEmpty() const { return root == nullptr; }

    bool isBalanced() const { return isBalanced(root); }

    int getHeight() const { return height(root); }

    void clear() {
        clear(root);
        root = nullptr;
    }
};

#endif // AVLTREE_H