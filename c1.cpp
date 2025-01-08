#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <nlohmann/json.hpp> // Include the nlohmann/json library
#include <fstream>

using namespace std;
using json = nlohmann::json; // Define the json alias

// Function declarations
json loadUserCredentials();
void saveUserCredentials(const json& userData);
void registerUser(int sock);

// Function definitions
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
        file << userData.dump(4); // Pretty-print with 4 spaces
    }
}

void registerUser(int sock) {
    string username, password;
    cout << "Enter a new username: ";
    cin >> username;
    cout << "Enter a new password: ";
    cin >> password;

    json userData = loadUserCredentials();
    if (userData.contains(username)) {
        cout << "Username already exists. Please choose a different username.\n";
        return;
    }

    userData[username] = password;
    saveUserCredentials(userData);

    cout << "Registration successful. You can now log in.\n";
}

void displayMainMenu() {
    cout << "\nMain Menu:\n";
    cout << "1. Login\n";
    cout << "2. Register\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

void displayClusterMenu() {
    cout << "\nCluster Menu:\n";
    cout << "1. Create Cluster\n";
    cout << "2. Edit Existing Cluster\n";
    cout << "3. Delete Cluster\n";
    cout << "4. View Clusters\n";
    cout << "5. View History\n";
    cout << "6. Log Out\n";
    cout << "Enter your choice: ";
}

void displayEditClusterMenu() {
    cout << "\nEdit Cluster Menu:\n";
    cout << "1. Add Data\n";
    cout << "2. Edit Data\n";
    cout << "3. Analyze Data\n";
    cout << "4. Delete Data\n";
    cout << "5. Go Back\n";
    cout << "Enter your choice: ";
}

void displayDataEntryInstructions() {
    cout << "\nAvailable Data Types and Entry Formats:\n";
    cout << "1. Circular Linked List: Enter values separated by spaces (e.g., 10 20 30).\n";
    cout << "2. Hashtable: Enter key-value pairs separated by commas (e.g., key1:value1,key2:value2).\n";
    cout << "3. Queue: Enter values separated by spaces (e.g., 10 20 30).\n";
    cout << "4. Binary Tree: Enter values separated by spaces (e.g., 15 10 20).\n";
    cout << "5. AVL Tree: Enter values separated by spaces (e.g., 15 10 20).\n";
    cout << "6. Graph: Enter edges in the format node1-node2,node3-node4 (e.g., A-B,C-D).\n";
    cout << "7. Heap: Enter values separated by spaces (e.g., 50 30 20).\n";
    cout << "Choose your data type and follow the instructions carefully.\n";
}

string sendToServer(int sock, const string& message) {
    send(sock, message.c_str(), message.size(), 0);

    char buffer[1024] = {0};
    recv(sock, buffer, 1024, 0);

    return string(buffer);
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Failed to create socket.\n";
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Failed to connect to server.\n";
        return 1;
    }

    cout << "Connected to server.\n";

    while (true) {
        displayMainMenu();
        int mainChoice;
        cin >> mainChoice;

        if (mainChoice == 1) { // Login
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            string loginMessage = "LOGIN " + username + " " + password;
            string loginResponse = sendToServer(sock, loginMessage);

            if (loginResponse == "LOGIN_SUCCESS") {
                cout << "Login successful.\n";

                while (true) {
                    displayClusterMenu();
                    int clusterChoice;
                    cin >> clusterChoice;

                    if (clusterChoice == 6) { // Log Out
                        string logoutMessage = "LOGOUT " + username;
                        string logoutResponse = sendToServer(sock, logoutMessage);
                        cout << logoutResponse << "\n";
                        break;
                    }

                    // Handle cluster menu choices...
                }
            } else {
                cout << "Login failed: " << loginResponse << "\n";
            }
        } else if (mainChoice == 2) { // Register
            registerUser(sock);
        } else if (mainChoice == 3) { // Exit
            cout << "Exiting...\n";
            close(sock);
            return 0;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    close(sock);
    return 0;
}