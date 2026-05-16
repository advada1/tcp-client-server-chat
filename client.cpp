#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4444       // Port number the client will connect to
#define BUFFER_SIZE 1024 // Buffer size for sending and receiving data

// Function to receive messages from the server
void receive_messages(int socket) {
    char buffer[BUFFER_SIZE]; // Buffer to store messages
    while (true) {
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
        int bytes_received = recv(socket, buffer, BUFFER_SIZE, 0); // Receive message from server
        if (bytes_received <= 0) { // If no data received or connection closed
            std::cout << "Disconnected from server." << std::endl; // Notify user of disconnection
            break; // Exit the loop
        }
        std::cout << buffer << std::endl; // Print the received message
    }
}

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments are passed
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <client_name>" << std::endl; // Error message
        return 1; // Exit program with error code
    }

    std::string server_ip = argv[1]; // First argument: server IP address
    std::string client_name = argv[2]; // Second argument: client name
    int client_socket; // Socket descriptor for the client
    struct sockaddr_in server_addr; // Struct to store server address information

    // Create a socket for communication
    client_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: IPv4, SOCK_STREAM: TCP
    if (client_socket == -1) { // Check if socket creation failed
        std::cerr << "Failed to create socket." << std::endl; // Error message
        return 1; // Exit program with error code
    }

    server_addr.sin_family = AF_INET; // Set address family to IPv4
    server_addr.sin_port = htons(PORT); // Convert port to network byte order
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) { // Convert IP address to binary
        std::cerr << "Invalid address." << std::endl; // Error message for invalid IP
        return 1; // Exit program with error code
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { // Attempt to connect
        std::cerr << "Connection failed." << std::endl; // Error message if connection fails
        return 1; // Exit program with error code
    }

    // Send client name to the server
    send(client_socket, client_name.c_str(), client_name.size(), 0); // Send the client name as the first message

    // Start a thread to handle receiving messages from the server
    std::thread receiver_thread(receive_messages, client_socket); // Create a thread for receiving messages
    receiver_thread.detach(); // Detach the thread to run independently

    char buffer[BUFFER_SIZE]; // Buffer to store user input
    while (true) {
        std::string input; // Variable to store user input
        std::getline(std::cin, input); // Get input from the user

        if (input == "exit") { // Check if the user wants to exit
            send(client_socket, input.c_str(), input.size(), 0); // Send the "exit" message to the server
            break; // Exit the loop
        }

        send(client_socket, input.c_str(), input.size(), 0); // Send the user input to the server
    }

    close(client_socket); // Close the socket after exiting the loop
    return 0; // Exit program successfully
}