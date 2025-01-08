# Non-Relational-Database-Using-server-and-client
This C++ client-server application enables users to manage data clusters using dynamic in-memory data structures such as Circular Linked Lists, Hash Tables, Queues, Binary Trees, AVL Trees, Graphs, and Heaps. It provides features like secure login, cluster creation, editing, and advanced data analysis, including sorting and graph traversal.

Instructions to Run the Project
To run this project, start by cloning the repository to your local machine using the command "git clone https://github.com/jalalcreates/Non-Relational-Database-Using-server-and-client" and navigate to the project directory with "cd Non-Relational-Database-Using-server-and-client". Ensure you have a C++ compiler installed, such as GCC or Clang, to compile the code.

First, compile the server file to generate the executable. Use the command "g++ -o server server.cpp -pthread" to compile the server. Once compiled, you can start the server by running "./server". The server will handle all the logic and operations for managing user data and clusters.

Next, compile the client file using "g++ -o client client.cpp". The client provides an interactive interface for users to log in, create clusters, and manage their data. To connect to the server, execute the client program with "./client". Ensure the client is running on a machine that has network access to the server’s host.

This setup allows the server to manage persistent storage and handle multiple client requests, while the client interacts with the server seamlessly over socket communication.
