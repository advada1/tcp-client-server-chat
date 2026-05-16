#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define PORT 4444
#define BUFFER_SIZE 1024

std::map<std::string, int> clients;   //map that store info on the clients
std::mutex clients_mutex;             //ensure thread-safe operations, allowing only one thread to access the data structure at a time


//communication with client
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];  //recive message from client
    std::string client_name;   //store client name

    // Receive client name from socket and store it in the buffer 
    memset(buffer, 0, BUFFER_SIZE);     //clear the buffer to insure no older data is present
    if (recv(client_socket, buffer, BUFFER_SIZE, 0) <= 0) {     //recive data from the socket
        close(client_socket);   //close the socket if no data is recived
        return;
    }
    client_name = buffer; //store the recive name

    // Register client
    {
        std::lock_guard<std::mutex> lock(clients_mutex); //ensure thread safety
        clients[client_name] = client_socket;  // Insert the client into the map.
    }

    std::cout << client_name << " connected." << std::endl;  //output messaage client is connected

    //recived messages from client
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);  // clear the buffer
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0); // recive message from client
        if (bytes_received <= 0) {  //if we didnt get the byte or error occurred, exit.
            break;
        }

        std::string message(buffer);  //convert buffer into string

        //if the message is "exit" break the loop and disconnect the client
        if (message == "exit") {
            std::cout << client_name << " disconnected." << std::endl;
            break;
        }

        // Parse message
        size_t space_pos = message.find(' ');       //Find the first space in the message 
        if (space_pos != std::string::npos && message.substr(0, space_pos) == "send") {     // Check if the command is "send".
            size_t second_space = message.find(' ', space_pos + 1);     // Find the next space after the target client name.
            if (second_space != std::string::npos) {        // Ensure there's a second space, which means the message has a target client and content.
                std::string target_client = message.substr(space_pos + 1, second_space - space_pos - 1);   // Extract the target client name.
                std::string msg_content = message.substr(second_space + 1);     // Extract the message content.

                // Send message to target client
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);    // Lock the mutex to ensure safe access to the `clients` map.
                    if (clients.find(target_client) != clients.end()) {     // Check if the target client exists in the map.
                        std::string incoming_message = "Incoming message: " + msg_content;  //the message we want to send
                        send(clients[target_client], incoming_message.c_str(), incoming_message.size(), 0);     //send the message to client
                    } else {        //if the client doesnt exist
                        std::string error_message = "Error: Client not found.";     //error
                        send(client_socket, error_message.c_str(), error_message.size(), 0);    //send error
                    }
                }
            }
        }
    }

    // Cleanup after client dissconnects
    {
        std::lock_guard<std::mutex> lock(clients_mutex);    //lock in the mutex to remove the client
        clients.erase(client_name);     //remove the client
    }
    close(client_socket);       //close the socket after we finished
}



int main() {
    int server_socket;  //socket descriptor for server
    struct sockaddr_in server_addr; //struct to hold server address info

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {      //if the socket creation fails
        std::cerr << "Failed to create socket." << std::endl;   //output error message
        return 1;       //exit the program
    }

    //set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;   // Allow connections from any IP address.
    server_addr.sin_port = htons(PORT);   // Set the port to listen on

    // Bind the socket to the server address.
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Binding failed." << std::endl;   // If binding fails, output an error.
        return 1;
    }

    // Listen for incoming client connections, with a backlog queue size of 10.
    if (listen(server_socket, 10) < 0) {        // If listening fails, output an error.
        std::cerr << "Listening failed." << std::endl;
        return 1;
    }

    std::cout << "Server is running on port " << PORT << std::endl;  // Output that the server is running.

     // Main server loop to accept client connections.
    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);  // Accept an incoming connection.
        if (client_socket < 0) {  // If the connection fails.
            std::cerr << "Failed to accept connection." << std::endl;  // Output an error message.
            continue;    // Continue the loop to accept other connections.
        }

        // Create a new thread to handle communication with the client.
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();    //Detach the thread to allow it to run independently.
    }

    close(server_socket);  // Close the server socket when the program ends.
    return 0;
}