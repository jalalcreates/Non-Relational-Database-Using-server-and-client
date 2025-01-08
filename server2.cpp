#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <thread>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Data Structures/CircularLinkedList.h"
#include "Data Structures/Hashtable.h"
#include "Data Structures/Queue.h"
#include "Data Structures/BinaryTree.h"
#include "Data Structures/AVLTree.h"
#include "Data Structures/Graph.h"
#include "Data Structures/Heap.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

// **Helper Functions**

/// **Users.json Management**
json loadUserCredentials() {
    ifstream file("users.json");
    json userData;
    if (file.is_open()) {
        file >> userData;
    }
    return userData;
}

void saveUserCredentials(const json& userData) {
    ofstream file("users.json");
    if (file.is_open()) {
        file << userData.dump(4);
    }
}

/// **Cluster Management**
void ensureClusterDirectoryExists(const string& username) {
    string userClusterPath = "clusters/" + username;
    if (!fs::exists(userClusterPath)) {
        fs::create_directories(userClusterPath);
    }
}
void saveHistory(const string& username, const string& action) {
    ofstream historyFile("history.txt", ios::app);
    if (historyFile.is_open()) {
        historyFile << username << ": " << action << "\n";
        historyFile.close();
    }
}
string getClusterFilePath(const string& username, const string& clusterName) {
    return "clusters/" + username + "/" + username + ".json";
}

json loadClusterData(const string& username, const string& clusterName) {
    string clusterPath = getClusterFilePath(username, clusterName);
    ifstream file(clusterPath);
    json clusterData;
    if (file.is_open()) {
        file >> clusterData;
    }
    return clusterData;
}

void saveClusterData(const string& username, const string& clusterName, const json& data) {
    ensureClusterDirectoryExists(username);
    string clusterPath = getClusterFilePath(username, clusterName);
    ofstream file(clusterPath);
    if (file.is_open()) {
        file << data.dump(4);
    }
}

void deleteClusterData(const string& username, const string& clusterName) {
    string clusterPath = getClusterFilePath(username, clusterName);
    if (fs::exists(clusterPath)) {
        fs::remove(clusterPath);
    }
}

vector<string> listClusters(const string& username) {
    ensureClusterDirectoryExists(username);
    vector<string> clusters;
    string userClusterPath = "clusters/" + username;

    for (const auto& entry : fs::directory_iterator(userClusterPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            clusters.push_back(entry.path().stem().string());
        }
    }
    return clusters;
}

/// **Command Handlers**

string handleLogin(const vector<string>& tokens, const string& clientIP) {
    if (tokens.size() != 3) return "INVALID_LOGIN_FORMAT";

    string username = tokens[1];
    string password = tokens[2];

    json userData = loadUserCredentials();

    if (userData.contains(username) && userData[username] == password) {
        return "LOGIN_SUCCESS";
    }

    return "LOGIN_FAILED";
}
string handleViewClusterData(const vector<string>& tokens, const string& username) {
    if (tokens.size() != 2) return "INVALID_VIEW_FORMAT";

    string clusterName = tokens[1];

    // Load cluster data from persistent storage
    json clusterData = loadClusterData(username, clusterName);
    if (clusterData.is_null()) {
        return "CLUSTER_NOT_FOUND";
    }

    // Debug: Print the loaded cluster data
    cout << "DEBUG: Loaded cluster data: " << clusterData.dump(4) << endl;

    // Extract data type and data
    string dataType = clusterData["dataType"];
    string data = clusterData["data"];

    // Format the data based on its type
    stringstream response;
    response << "Cluster: " << clusterName << "\n";
    response << "Data Type: " << dataType << "\n";
    response << "Data: " << data << "\n";

    return response.str();
}
string handleRegister(const vector<string>& tokens) {
    if (tokens.size() != 3) return "INVALID_REGISTER_FORMAT";

    string username = tokens[1];
    string password = tokens[2];

    json userData = loadUserCredentials();
    if (userData.contains(username)) {
        return "USERNAME_ALREADY_EXISTS";
    }

    userData[username] = password;
    saveUserCredentials(userData);

    return "REGISTRATION_SUCCESS";
}
string handleCreateCluster(const vector<string>& tokens, const string& username) {
    if (tokens.size() != 3) return "INVALID_CREATE_FORMAT";

    string clusterName = tokens[1];
    string clusterPath = getClusterFilePath(username, clusterName);

    if (fs::exists(clusterPath)) {
        return "CLUSTER_ALREADY_EXISTS";
    }

    json clusterData = json::object();
    saveClusterData(username, clusterName, clusterData);
    return "CLUSTER_CREATED";
}

string handleDeleteCluster(const vector<string>& tokens, const string& username) {
    if (tokens.size() != 2) return "INVALID_DELETE_FORMAT";

    string clusterName = tokens[1];
    string clusterPath = getClusterFilePath(username, clusterName);

    if (!fs::exists(clusterPath)) {
        return "CLUSTER_NOT_FOUND";
    }

    deleteClusterData(username, clusterName);
    return "CLUSTER_DELETED";
}

string handleListClusters(const string& username) {
    vector<string> clusters = listClusters(username);
    if (clusters.empty()) {
        return "NO_CLUSTERS_FOUND";
    }

    stringstream response;
    for (const auto& cluster : clusters) {
        response << cluster << "\n";
    }
    return response.str();
}

string handleAddData(const vector<string>& tokens, const string& username) {
    if (tokens.size() < 4) return "INVALID_ADD_FORMAT";

    string clusterName = tokens[1];
    string dataType = tokens[2];
    string data = tokens[3];

    // Load cluster data from persistent storage
    json clusterData = loadClusterData(username, clusterName);
    if (clusterData.is_null()) return "CLUSTER_NOT_FOUND";

    // Handle data based on its type and store it in memory
    if (dataType == "CircularLinkedList") {
        CircularLinkedList<string> linkedList;
        if (clusterData.contains("data")) {
            stringstream ss(clusterData["data"].get<string>());
            string value;
            while (ss >> value) linkedList.insert(value);
        }
        stringstream ss(data);
        string value;
        while (ss >> value) linkedList.insert(value);
        clusterData["dataType"] = "CircularLinkedList";
        clusterData["data"] = linkedList.asString();
    } else if (dataType == "Hashtable") {
        HashTable<string, string> hashtable;
        if (clusterData.contains("data")) {
            stringstream ss(clusterData["data"].get<string>());
            string pair;
            while (getline(ss, pair, ',')) {
                size_t pos = pair.find(':');
                if (pos != string::npos) {
                    string key = pair.substr(0, pos);
                    string value = pair.substr(pos + 1);
                    hashtable.insert(key, value);
                }
            }
        }
        stringstream ss(data);
        string pair;
        while (getline(ss, pair, ',')) {
            size_t pos = pair.find(':');
            if (pos != string::npos) {
                string key = pair.substr(0, pos);
                string value = pair.substr(pos + 1);
                hashtable.insert(key, value);
            }
        }
        clusterData["dataType"] = "Hashtable";
        clusterData["data"] = hashtable.asString();
    } else if (dataType == "Queue") {
        Queue<string> queue;
        if (clusterData.contains("data")) {
            stringstream ss(clusterData["data"].get<string>());
            string value;
            while (ss >> value) queue.enqueue(value);
        }
        stringstream ss(data);
        string value;
        while (ss >> value) queue.enqueue(value);
        clusterData["dataType"] = "Queue";
        clusterData["data"] = queue.asString();
    } else if (dataType == "BinaryTree") {
        BinaryTree<int> binaryTree;
        if (clusterData.contains("data")) {
            stringstream ss(clusterData["data"].get<string>());
            int value;
            while (ss >> value) binaryTree.insert(value);
        }
        stringstream ss(data);
        int value;
        while (ss >> value) binaryTree.insert(value);
        clusterData["dataType"] = "BinaryTree";
        clusterData["data"] = binaryTree.inorderAsString();
    } else if (dataType == "AVLTree") {
        AVLTree<int> avlTree;
        if (clusterData.contains("data")) {
            stringstream ss(clusterData["data"].get<string>());
            int value;
            while (ss >> value) avlTree.insert(value);
        }
        stringstream ss(data);
        int value;
        while (ss >> value) avlTree.insert(value);
        clusterData["dataType"] = "AVLTree";
        clusterData["data"] = "AVLTree updated with new values";
    } else if (dataType == "Graph") {
        Graph<string> graph;
        if (clusterData.contains("data")) {
            stringstream ss(clusterData["data"].get<string>());
            string edge;
            while (getline(ss, edge, ',')) {
                size_t pos = edge.find('-');
                if (pos != string::npos) {
                    string node1 = edge.substr(0, pos);
                    string node2 = edge.substr(pos + 1);
                    graph.addEdge(node1, node2);
                }
            }
        }
        stringstream ss(data);
        string edge;
        while (getline(ss, edge, ',')) {
            size_t pos = edge.find('-');
            if (pos != string::npos) {
                string node1 = edge.substr(0, pos);
                string node2 = edge.substr(pos + 1);
                graph.addEdge(node1, node2);
            }
        }
        clusterData["dataType"] = "Graph";
        clusterData["data"] = "Graph updated with new edges";
    } else if (dataType == "Heap") {
        Heap<int> heap;
        if (clusterData.contains("data")) {
            stringstream ss(clusterData["data"].get<string>());
            int value;
            while (ss >> value) heap.insert(value);
        }
        stringstream ss(data);
        int value;
        while (ss >> value) heap.insert(value);
        clusterData["dataType"] = "Heap";
        clusterData["data"] = "Heap updated with new values";
    } else {
        return "DATA_TYPE_NOT_SUPPORTED";
    }

    // Save updated data to persistent storage
    saveClusterData(username, clusterName, clusterData);

    // Record the addition in history
    saveHistory(username, "Data added to cluster " + clusterName + ": " + data);

    return "DATA_ADDED";
}



string handleAnalyzeData(const vector<string>& tokens, const string& username) {
    if (tokens.size() < 3) return "INVALID_ANALYZE_FORMAT";

    string clusterName = tokens[1];
    string analysisType = tokens[2];

    // Load cluster data from persistent storage
    json clusterData = loadClusterData(username, clusterName);
    if (clusterData.is_null()) return "CLUSTER_NOT_FOUND";

    string dataType = clusterData["dataType"];
    string data = clusterData["data"];

    // Circular Linked List Analysis
    if (dataType == "CircularLinkedList") {
        CircularLinkedList<string> linkedList;
        stringstream ss(data);
        string value;
        while (ss >> value) linkedList.insert(value);

        if (analysisType == "sort") {
            vector<string> items;
            while (!linkedList.isEmpty()) {
                items.push_back(linkedList.remove());
            }
            sort(items.begin(), items.end());
            return "Sorted data: " + accumulate(items.begin(), items.end(), string(""), 
                [](const string& a, const string& b) { return a + (a.empty() ? "" : ", ") + b; });
        }
    }

    // Hashtable Analysis
    else if (dataType == "Hashtable") {
        HashTable<string, string> hashtable;
        stringstream ss(data);
        string pair;
        while (getline(ss, pair, ',')) {
            size_t pos = pair.find(':');
            if (pos != string::npos) {
                string key = pair.substr(0, pos);
                string value = pair.substr(pos + 1);
                hashtable.insert(key, value);
            }
        }

        if (analysisType == "count") {
            return "Total keys: " + to_string(hashtable.getSize());
        } else if (analysisType == "keys") {
            return "Keys: " + hashtable.asString();
        }
    }

    // Queue Analysis
    else if (dataType == "Queue") {
        Queue<string> queue;
        stringstream ss(data);
        string value;
        while (ss >> value) queue.enqueue(value);

        if (analysisType == "size") {
            return "Queue size: " + to_string(queue.size());
        } else if (analysisType == "peek") {
            if (queue.isEmpty()) return "Queue is empty";
            return "Front of the queue: " + queue.peek();
        }
    }

    // Binary Tree Analysis
    else if (dataType == "BinaryTree") {
        BinaryTree<int> binaryTree;
        stringstream ss(data);
        int value;
        while (ss >> value) binaryTree.insert(value);

        if (analysisType == "inorder") {
            return "Inorder traversal: " + binaryTree.inorderAsString();
        } else if (analysisType == "max") {
            return "Maximum value: " + to_string(binaryTree.findMax());
        }
    }

    // AVL Tree Analysis
    else if (dataType == "AVLTree") {
        AVLTree<int> avlTree;
        stringstream ss(data);
        int value;
        while (ss >> value) avlTree.insert(value);

        if (analysisType == "height") {
            return "Tree height: " + to_string(avlTree.getHeight());
        } else if (analysisType == "balanced") {
            return avlTree.isBalanced() ? "Tree is balanced" : "Tree is not balanced";
        }
    }

    // Graph Analysis
    else if (dataType == "Graph") {
        Graph<string> graph;
        stringstream ss(data);
        string edge;
        while (getline(ss, edge, ',')) {
            size_t pos = edge.find('-');
            if (pos != string::npos) {
                string node1 = edge.substr(0, pos);
                string node2 = edge.substr(pos + 1);
                graph.addEdge(node1, node2);
            }
        }

        if (analysisType == "bfs") {
            string startNode;
            cout << "Enter start node for BFS: ";
            cin >> startNode;
            return "BFS traversal: " + graph.bfsAsString(startNode);
        }
    }

    // Heap Analysis
    else if (dataType == "Heap") {
        Heap<int> heap;
        stringstream ss(data);
        int value;
        while (ss >> value) heap.insert(value);

        if (analysisType == "max") {
            return "Maximum value: " + to_string(heap.findMax());
        } else if (analysisType == "heapify") {
            return "Heapified values: " + heap.asString();
        }
    }

    return "ANALYSIS_NOT_SUPPORTED_FOR_DATATYPE";
}


string handleEditData(const vector<string>& tokens, const string& username) {
    if (tokens.size() < 5) return "INVALID_EDIT_FORMAT";

    string clusterName = tokens[1];
    string fileName = tokens[2]; // Not used but part of original function
    string key = tokens[3];
    string newValue = tokens[4];

    // Load cluster data from persistent storage
    json clusterData = loadClusterData(username, clusterName);
    if (clusterData.is_null()) {
        return "CLUSTER_NOT_FOUND";
    }

    string dataType = clusterData["dataType"];

    // Editing for Hashtable
    if (dataType == "Hashtable") {
        HashTable<string, string> hashtable;
        stringstream ss(clusterData["data"].get<string>());
        string pair;
        while (getline(ss, pair, ',')) {
            size_t pos = pair.find(':');
            if (pos != string::npos) {
                string currentKey = pair.substr(0, pos);
                string value = pair.substr(pos + 1);
                hashtable.insert(currentKey, value);
            }
        }

        if (hashtable.contains(key)) {
            hashtable.insert(key, newValue);
            clusterData["data"] = hashtable.asString();
            saveClusterData(username, clusterName, clusterData);
            saveHistory(username, "Edited key " + key + " in cluster " + clusterName);
            return "DATA_EDITED";
        } else {
            return "KEY_NOT_FOUND";
        }
    }

    // Editing for CircularLinkedList
    else if (dataType == "CircularLinkedList") {
        CircularLinkedList<string> linkedList;
        stringstream ss(clusterData["data"].get<string>());
        string value;
        while (ss >> value) linkedList.insert(value);

        // Find and replace the element (Linked List doesn't have key-value pair)
        bool found = false;
        while (!linkedList.isEmpty()) {
            string current = linkedList.remove();
            if (current == key) {
                linkedList.insert(newValue);
                found = true;
            } else {
                linkedList.insert(current);  // Insert back if not modified
            }
        }

        if (found) {
            clusterData["data"] = linkedList.asString();
            saveClusterData(username, clusterName, clusterData);
            saveHistory(username, "Edited value in CircularLinkedList in cluster " + clusterName);
            return "DATA_EDITED";
        } else {
            return "KEY_NOT_FOUND";
        }
    }

    // Editing for Queue
    else if (dataType == "Queue") {
        Queue<string> queue;
        stringstream ss(clusterData["data"].get<string>());
        string value;
        while (ss >> value) queue.enqueue(value);

        // Find and replace the element (Queue operates on a first-in-first-out basis)
        bool found = false;
        size_t size = queue.size();
        for (size_t i = 0; i < size; i++) {
            string current = queue.dequeue();
            if (current == key) {
                queue.enqueue(newValue);
                found = true;
            } else {
                queue.enqueue(current);
            }
        }

        if (found) {
            clusterData["data"] = queue.asString();
            saveClusterData(username, clusterName, clusterData);
            saveHistory(username, "Edited value in Queue in cluster " + clusterName);
            return "DATA_EDITED";
        } else {
            return "KEY_NOT_FOUND";
        }
    }

    // Editing for BinaryTree
    else if (dataType == "BinaryTree") {
        BinaryTree<int> binaryTree;
        stringstream ss(clusterData["data"].get<string>());
        int value;
        while (ss >> value) binaryTree.insert(value);

        // Find and replace the value in the BinaryTree
        // Here we assume the tree doesn't support direct key modification, so we remove and reinsert
        binaryTree.remove(stoi(key)); // Assuming 'key' is the value to be removed
        binaryTree.insert(stoi(newValue));

        clusterData["data"] = binaryTree.inorderAsString();
        saveClusterData(username, clusterName, clusterData);
        saveHistory(username, "Edited value in BinaryTree in cluster " + clusterName);
        return "DATA_EDITED";
    }

    // Editing for AVLTree
    else if (dataType == "AVLTree") {
        AVLTree<int> avlTree;
        stringstream ss(clusterData["data"].get<string>());
        int value;
        while (ss >> value) avlTree.insert(value);

        // Find and replace the value in the AVLTree
        avlTree.remove(stoi(key)); // Assuming 'key' is the value to be removed
        avlTree.insert(stoi(newValue));

        clusterData["data"] = "AVLTree updated with new value";
        saveClusterData(username, clusterName, clusterData);
        saveHistory(username, "Edited value in AVLTree in cluster " + clusterName);
        return "DATA_EDITED";
    }

    // Editing for Graph
    else if (dataType == "Graph") {
        Graph<string> graph;
        stringstream ss(clusterData["data"].get<string>());
        string edge;
        while (getline(ss, edge, ',')) {
            size_t pos = edge.find('-');
            if (pos != string::npos) {
                string node1 = edge.substr(0, pos);
                string node2 = edge.substr(pos + 1);
                graph.addEdge(node1, node2);
            }
        }

        // Find and replace the edge in the Graph (if applicable)
        bool found = false;
        stringstream newEdges;
        while (getline(ss, edge, ',')) {
            size_t pos = edge.find('-');
            if (pos != string::npos) {
                string node1 = edge.substr(0, pos);
                string node2 = edge.substr(pos + 1);
                if (node1 == key || node2 == key) {
                    newEdges << node1 << "-" << newValue << ",";
                    found = true;
                } else {
                    newEdges << edge << ",";
                }
            }
        }

        if (found) {
            clusterData["data"] = newEdges.str();
            saveClusterData(username, clusterName, clusterData);
            saveHistory(username, "Edited edge in Graph in cluster " + clusterName);
            return "DATA_EDITED";
        } else {
            return "KEY_NOT_FOUND";
        }
    }

    // Editing for Heap
    else if (dataType == "Heap") {
        Heap<int> heap;
        stringstream ss(clusterData["data"].get<string>());
        int value;
        while (ss >> value) heap.insert(value);

        // Find and replace the value in the Heap
        heap.remove(stoi(key));  // Assuming 'key' is the value to be removed
        heap.insert(stoi(newValue));

        clusterData["data"] = "Heap updated with new value";
        saveClusterData(username, clusterName, clusterData);
        saveHistory(username, "Edited value in Heap in cluster " + clusterName);
        return "DATA_EDITED";
    }

    return "EDIT_OPERATION_NOT_SUPPORTED_FOR_DATATYPE";
}

string handleCheckCluster(const vector<string>& tokens, const string& username) {
    if (tokens.size() != 2) return "INVALID_CHECK_CLUSTER_FORMAT";

    string clusterName = tokens[1];
    string clusterPath = getClusterFilePath(username, clusterName);

    if (fs::exists(clusterPath)) {
        return "CLUSTER_FOUND";
    }

    return "CLUSTER_NOT_FOUND";
}
string handleLogout(const vector<string>& tokens) {
    if (tokens.size() != 2) return "INVALID_LOGOUT_FORMAT";

    string username = tokens[1];
    saveHistory(username, "User logged out");

    return "LOGOUT_SUCCESS";
}
string handleDeleteData(const vector<string>& tokens, const string& username) {
    if (tokens.size() != 4) return "INVALID_DELETE_DATA_FORMAT";

    string clusterName = tokens[1];
    string fileName = tokens[2]; // Not used but part of original function
    string password = tokens[3];

    // Verify password
    json userData = loadUserCredentials();
    if (!userData.contains(username) || userData[username] != password) {
        return "INVALID_PASSWORD";
    }

    // Load cluster data
    json clusterData = loadClusterData(username, clusterName);
    if (clusterData.is_null()) {
        return "CLUSTER_NOT_FOUND";
    }

    // Clear data
    clusterData["data"] = "";
    saveClusterData(username, clusterName, clusterData);

    saveHistory(username, "Data deleted from cluster " + clusterName);

    return "DATA_DELETED";
}
string handleClientQuery(const string& query, const string& clientIP) {
    cout << "DEBUG: Received query: " << query << endl;
    stringstream ss(query);
    string command;
    vector<string> tokens;

    // Tokenize the input query
    while (ss >> command) tokens.push_back(command);
    if (tokens.empty()) return "EMPTY_QUERY";

    // Extract username (if available)
    string username = tokens.size() > 1 ? tokens[1] : "";

    // Command routing
    if (tokens[0] == "LOGIN") {
        return handleLogin(tokens, clientIP);
    } 
    else if (tokens[0] == "CHECK_CLUSTER") {
    if (tokens.size() < 2) return "INVALID_CHECK_CLUSTER_FORMAT";
    return handleCheckCluster(tokens, username);
}
    else if (tokens[0] == "CREATE_CLUSTER") {
        if (tokens.size() < 3) return "INVALID_CREATE_CLUSTER_FORMAT";
        return handleCreateCluster(tokens, username);
    } 
    else if (tokens[0] == "REGISTER") {
    if (tokens.size() < 3) return "INVALID_REGISTER_FORMAT";
    return handleRegister(tokens);
}
    else if (tokens[0] == "DELETE_CLUSTER") {
        if (tokens.size() < 3) return "INVALID_DELETE_CLUSTER_FORMAT";
        return handleDeleteCluster(tokens, username);
    } 
    else if (tokens[0] == "LIST_CLUSTERS") {
        return handleListClusters(username);
    } 

    else if (tokens[0] == "ADD_DATA") {
        if (tokens.size() < 4) return "INVALID_ADD_DATA_FORMAT";
        return handleAddData(tokens, username);
    } 
    else if (tokens[0] == "VIEW_CLUSTER_DATA") {
    if (tokens.size() < 2) return "INVALID_VIEW_CLUSTER_DATA_FORMAT";
    return handleViewClusterData(tokens, username);
}
    else if (tokens[0] == "EDIT_DATA") {
        if (tokens.size() < 5) return "INVALID_EDIT_DATA_FORMAT";
        return handleEditData(tokens, username);
    } 
    else if (tokens[0] == "LOGOUT") {
    if (tokens.size() < 2) return "INVALID_LOGOUT_FORMAT";
    return handleLogout(tokens);
}
else if (tokens[0] == "DELETE_DATA") {
    if (tokens.size() < 4) return "INVALID_DELETE_DATA_FORMAT";
    return handleDeleteData(tokens, username);
}
    else if (tokens[0] == "ANALYZE_DATA") {
        if (tokens.size() < 3) return "INVALID_ANALYZE_DATA_FORMAT";
        return handleAnalyzeData(tokens, username);
    } 

    return "UNKNOWN_COMMAND";
}


// **Client Handling**
void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr*)&clientAddr, &addrLen);
    string clientIP = inet_ntoa(clientAddr.sin_addr);

    while (true) {
        memset(buffer, 0, 1024);
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
        if (bytesReceived <= 0) break;

        string query(buffer);
        cout << "Received query: " << query << " from " << clientIP << endl;

        string response = handleClientQuery(query, clientIP);
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    close(clientSocket);
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == -1) {
        cerr << "Socket creation failed.\n";
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Binding failed.\n";
        return 1;
    }

    if (listen(serverSock, 5) < 0) {
        cerr << "Listening failed.\n";
        return 1;
    }

    cout << "Server is listening on port 8080...\n";

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSock < 0) {
            cerr << "Failed to accept client connection.\n";
            continue;
        }

        cout << "Client connected.\n";
        thread(handleClient, clientSock).detach();
    }

    close(serverSock);
    return 0;
}

