#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <vector>
#include <list>
#include <stdexcept>
#include <sstream>
using namespace std;

template <typename K, typename V, typename Hash = std::hash<K>>
class HashTable {
private:
    struct Entry {
        K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

    vector<list<Entry>> table;
    size_t capacity;
    size_t size;
    float loadFactor;
    Hash hashFunc;

    size_t hash(const K& key) const {
        return hashFunc(key) % capacity;
    }

    void rehash() {
        size_t newCapacity = capacity * 2;
        vector<list<Entry>> newTable(newCapacity);
        for (size_t i = 0; i < capacity; ++i) {
            for (const auto& entry : table[i]) {
                size_t newIndex = hash(entry.key) % newCapacity;
                newTable[newIndex].emplace_back(entry.key, entry.value);
            }
        }
        table = std::move(newTable);
        capacity = newCapacity;
    }

public:
    HashTable(size_t cap = 10, float lf = 0.75, Hash hashFunc = Hash())
        : capacity(cap), size(0), loadFactor(lf), hashFunc(hashFunc) {
        table.resize(capacity);
    }

    void insert(const K& key, const V& value) {
        if (size >= capacity * loadFactor) {
            rehash();
        }

        size_t index = hash(key);
        for (auto& entry : table[index]) {
            if (entry.key == key) {
                entry.value = value;
                return;
            }
        }
        table[index].emplace_back(key, value);
        ++size;
    }

    void remove(const K& key) {
        size_t index = hash(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->key == key) {
                table[index].erase(it);
                --size;
                return;
            }
        }
        throw invalid_argument("Key not found.");
    }

    V get(const K& key) const {
        size_t index = hash(key);
        for (const auto& entry : table[index]) {
            if (entry.key == key) return entry.value;
        }
        throw invalid_argument("Key not found.");
    }

    bool contains(const K& key) const {
        size_t index = hash(key);
        for (const auto& entry : table[index]) {
            if (entry.key == key) return true;
        }
        return false;
    }

    void display() const {
        for (size_t i = 0; i < capacity; ++i) {
            cout << "Bucket " << i << ": ";
            for (const auto& entry : table[i]) {
                cout << "[" << entry.key << ": " << entry.value << "] ";
            }
            cout << endl;
        }
    }

    size_t getSize() const {
        return size;
    }

    size_t getCapacity() const {
        return capacity;
    }

    void clear() {
        for (size_t i = 0; i < capacity; ++i) {
            table[i].clear();
        }
        size = 0;
    }

    bool isEmpty() const {
        return size == 0;
    }

    string asString() const {
        ostringstream oss;
        for (size_t i = 0; i < capacity; ++i) {
            for (const auto& entry : table[i]) {
                oss << entry.key << ":" << entry.value << ",";
            }
        }
        string result = oss.str();
        if (!result.empty()) {
            result.pop_back(); // Remove the trailing comma
        }
        return result;
    }

    Entry* find(const K& key) {
        size_t index = hash(key);
        for (auto& entry : table[index]) {
            if (entry.key == key) return &entry;
        }
        return nullptr;
    }

    V& operator[](const K& key) {
        size_t index = hash(key);
        for (auto& entry : table[index]) {
            if (entry.key == key) return entry.value;
        }
        table[index].emplace_back(key, V());
        ++size;
        return table[index].back().value;
    }

    vector<K> getKeys() const {
        vector<K> keys;
        for (size_t i = 0; i < capacity; ++i) {
            for (const auto& entry : table[i]) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }

    vector<V> getValues() const {
        vector<V> values;
        for (size_t i = 0; i < capacity; ++i) {
            for (const auto& entry : table[i]) {
                values.push_back(entry.value);
            }
        }
        return values;
    }
};

#endif // HASHTABLE_H