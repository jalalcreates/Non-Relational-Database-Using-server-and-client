#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;

template <typename T>
class Graph {
private:
    unordered_map<T, vector<T>> adjList;

public:
    // Add an edge to the graph
    void addEdge(const T& u, const T& v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u); // Undirected graph
    }

    // Add a node to the graph
    void addNode(const T& node) {
        if (adjList.find(node) == adjList.end()) {
            adjList[node] = vector<T>();
        }
    }

    // Remove a node from the graph
    void removeNode(const T& node) {
        if (adjList.find(node) == adjList.end()) return;

        // Remove all edges to the node
        for (auto& pair : adjList) {
            auto& neighbors = pair.second;
            neighbors.erase(remove(neighbors.begin(), neighbors.end(), node), neighbors.end());
        }

        // Remove the node
        adjList.erase(node);
    }

    // Remove an edge from the graph
    void removeEdge(const T& u, const T& v) {
        if (adjList.find(u) == adjList.end() || adjList.find(v) == adjList.end()) return;

        // Remove v from u's neighbors
        auto& uNeighbors = adjList[u];
        uNeighbors.erase(remove(uNeighbors.begin(), uNeighbors.end(), v), uNeighbors.end());

        // Remove u from v's neighbors
        auto& vNeighbors = adjList[v];
        vNeighbors.erase(remove(vNeighbors.begin(), vNeighbors.end(), u), vNeighbors.end());
    }

    // Display the adjacency list representation of the graph
    void display() const {
        for (const auto& pair : adjList) {
            cout << pair.first << ": ";
            for (const auto& neighbor : pair.second) {
                cout << neighbor << " ";
            }
            cout << endl;
        }
    }

    // Perform BFS traversal from a given start node
    void bfs(const T& start) const {
        if (adjList.find(start) == adjList.end()) {
            throw invalid_argument("Start node not found in the graph.");
        }

        unordered_map<T, bool> visited;
        queue<T> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            T node = q.front();
            q.pop();
            cout << node << " ";

            for (const auto& neighbor : adjList.at(node)) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        cout << endl;
    }

    // Perform BFS traversal and return the result as a string
    string bfsAsString(const T& start) const {
        if (adjList.find(start) == adjList.end()) {
            throw invalid_argument("Start node not found in the graph.");
        }

        unordered_map<T, bool> visited;
        queue<T> q;
        ostringstream result;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            T node = q.front();
            q.pop();
            result << node << " ";

            for (const auto& neighbor : adjList.at(node)) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }

        return result.str();
    }

    // Check if a node exists in the graph
    bool containsNode(const T& node) const {
        return adjList.find(node) != adjList.end();
    }

    // Get the neighbors of a given node
    vector<T> getNeighbors(const T& node) const {
        if (adjList.find(node) == adjList.end()) {
            throw invalid_argument("Node not found in the graph.");
        }
        return adjList.at(node);
    }

    // Get the size of the graph (number of nodes)
    size_t size() const {
        return adjList.size();
    }

    // Check if the graph is connected
    bool isConnected() const {
        if (adjList.empty()) return true;

        unordered_map<T, bool> visited;
        queue<T> q;

        // Start BFS from the first node
        auto startNode = adjList.begin()->first;
        visited[startNode] = true;
        q.push(startNode);

        while (!q.empty()) {
            T node = q.front();
            q.pop();

            for (const auto& neighbor : adjList.at(node)) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }

        // Check if all nodes are visited
        for (const auto& pair : adjList) {
            if (!visited[pair.first]) return false;
        }

        return true;
    }

    // Return the graph as a string
    string asString() const {
        ostringstream oss;
        for (const auto& pair : adjList) {
            oss << pair.first << ": ";
            for (const auto& neighbor : pair.second) {
                oss << neighbor << " ";
            }
            oss << "\n";
        }
        return oss.str();
    }
};

#endif // GRAPH_H