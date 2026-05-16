# TCP Client-Server Chat Application

A multithreaded client-server chat application written in C++ as part of an academic Computer Science project.

## Technologies
- C++
- TCP sockets
- Multithreading
- Client-server architecture

## Features
- Allows multiple clients to connect to a central server
- Registers each client by name
- Supports direct messages between clients using the command:
  send <client_name> <message>
- Handles each client connection in a separate thread
- Uses mutex locking for thread-safe access to connected clients

## How to Run

Compile the server:
g++ server.cpp -o server -pthread

Compile the client:
g++ client.cpp -o client -pthread

Run the server:
./server

Run a client:
./client <server_ip> <client_name>

Example:
./client 127.0.0.1 Adva
