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
- Supports direct messages between clients using the command: `send <client_name> <message>`
- Handles each client connection in a separate thread
- Uses mutex locking for thread-safe access to connected clients

## How to Run

Compile the server:

```bash
g++ server.cpp -o server -pthread
```

Compile the client:

```bash
g++ client.cpp -o client -pthread
```

Run the server:

```bash
./server
```

Run a client:

```bash
./client <server_ip> <client_name>
```

Example:

```bash
./client 127.0.0.1 Adva
```

## Usage

After running the server and connecting multiple clients, send a direct message using:

```bash
send <client_name> <message>
```

Example:

```bash
send David Hello, how are you?
```
