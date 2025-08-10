#include "../include/Server.h"
#include "../include/Utils.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/epoll.h>  // *** epoll change ***

#define BUFFER_SIZE 1024

/*
This is the member initializer list — it sets class members before the constructor body runs:
port(port) → Assigns the passed argument port to the member variable port.
server_fd(-1) → Initializes server_fd to -1 (meaning “no valid socket yet”).
max_fd(0) → Initializes max_fd to 0 (no sockets tracked yet).
epoll_fd(-1) → Initializes epoll_fd to -1 (no epoll instance yet). *** epoll change ***
*/
TCPServer :: TCPServer(int port) : port(port), server_fd(-1), epoll_fd(-1) {
    // FD_ZERO(&master_set);  // *** removed because no select()
}

TCPServer::~TCPServer() {
    if (server_fd != -1) close(server_fd);
    if (epoll_fd != -1) close(epoll_fd);  // *** epoll change ***
}

/*
======================================================================================
server_fd = socket(AF_INET, SOCK_STREAM, 0);
=======================================================================================

1.AF_INET

Address family → Use IPv4 addresses (AF_INET6 would be for IPv6).
This tells the OS that the socket will work with IPv4 Internet protocols.
----------------------------------------------------------------------------------------

2.SOCK_STREAM

Socket type → This means we want a stream socket, which is connection-oriented (TCP).
Alternative: SOCK_DGRAM would be for datagram sockets (UDP).
--------------------------------------------------------------------------------------------

3.0

Protocol → 0 means “choose the default protocol for this address family and socket type.”

In this case, IPv4 + SOCK_STREAM defaults to TCP.
*/


void TCPServer :: setupServerSocket(){

    //STEP-1: Creating SOCKET
    server_fd = socket(AF_INET, SOCK_STREAM, 0);  //Creates a TCP socket using IPv4 (AF_INET) and returns its file descriptor, storing it in server_fd.
    if (server_fd < 0) Utils::errorExit("Socket failed");

    /*
    We can use server as mnay times we want.
    Why SO_REUSEADDR works 
    It tells the OS:
    “I know this port might still be in TIME_WAIT, but I want to reuse it immediately.”
    Common for servers during development so you don’t have to wait minutes to restart.
    */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    sockaddr_in server_addr{};  //Creates and zero-initializes a sockaddr_in structure, which will hold the server’s address and port.
    server_addr.sin_family = AF_INET;  //Sets the address family to IPv4 for the socket.
    server_addr.sin_addr.s_addr = INADDR_ANY;   //Tells the server to listen on all available network interfaces.
    server_addr.sin_port = htons(port);   //Converts the server port number to network byte order and stores it in the struct.

    //STEP-2: BINDING: Assigns the IP address and port to the socket; if binding fails, print error and exit.
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        Utils::errorExit("Bind failed");

    //STEP-3 LISTENING: Puts the socket into listening mode with a backlog queue size of 5; exits on failure.
    if (listen(server_fd, 5) < 0)
        Utils::errorExit("Listen failed");

    // *** epoll change: create epoll instance ***
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        Utils::errorExit("epoll_create1 failed");
    }

    // *** epoll change: register server_fd for EPOLLIN events ***
    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        Utils::errorExit("epoll_ctl: server_fd");
    }

    std::cout << "🔌 Server listening on port " << port << "...\n";
}

//STEP-4 Accept Client or new Connection(BLOCKING)
void TCPServer :: handleNewConnection(){

    sockaddr_in client_addr{};  //Creates and zero-initializes a sockaddr_in struct to store the connecting client’s IP and port.
    socklen_t addr_len = sizeof(client_addr);  //Sets the variable addr_len to the size of client_addr; accept() will update it with the actual size of the returned address.


    std::cout << "⏳ Waiting for client to connect (accept blocks here)...\n";
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);  //Accepts a new incoming TCP connection on server_fd, returning a new socket file descriptor (client_fd) for communicating with that client.
    
    //If accept() failed (negative return), print the error and return without adding the client to the tracking set.
    if (client_fd < 0) {
        perror("Accept failed");
        return;
    }

    // *** epoll change: register new client socket for EPOLLIN ***
    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        perror("epoll_ctl: client_fd");
        close(client_fd);
        return;
    }

    std::cout << "✅ New client connected (FD: " << client_fd << ")\n";
}


//STEP-5 : Receive message and send Reply
void TCPServer :: handleClientMessage(int client_fd){
    char buffer[BUFFER_SIZE] = {0};  //Creates a character array of size BUFFER_SIZE, initialized with zeros, to store incoming data from the client.
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);  //Reads up to BUFFER_SIZE bytes from the client socket into buffer, returning the number of bytes actually received.

    //Checks if the client closed the connection (0) or an error occurred (-1).
    if (bytes_received <= 0) {
        std::cout << "❌ Client disconnected (FD: " << client_fd << ")\n";
        closeClient(client_fd);
        return;
    }

    std::cout << "📩 Received from FD " << client_fd << ": " << buffer;

    // Echo back
    send(client_fd, buffer, bytes_received, 0);  //Sends the exact same data back to the client, implementing an echo server behavior.
}   

//STEP-7 : Close sockets
void TCPServer::closeClient(int client_fd) {
    close(client_fd);

    // *** epoll change: remove fd from epoll instance ***
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr) == -1) {
        perror("epoll_ctl: DEL client_fd");
    }
}


void TCPServer::start() {
    // Initializes the server socket, binds it to the port, starts listening, and adds it to master_set.
    setupServerSocket();

    epoll_event events[MAX_EVENTS];  // *** epoll change: buffer for epoll events ***

    // Enters the main infinite event loop that continuously checks for socket activity.
    while (true) {
        // *** epoll change: wait for events ***
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (event_count == -1) {
            perror("epoll_wait");
            break;
        }

        //Loops through all the ready events returned by epoll_wait.
        for (int i = 0; i < event_count; i++) {
            int fd = events[i].data.fd;

            //Checks if the event is on the server socket → accept a new client;
            //otherwise → read data from an existing client and process it.
            if (fd == server_fd) {
                handleNewConnection();
            } else {
                handleClientMessage(fd);
            }
        }
    }
}

/*
This is your event loop.
select() watches all sockets in master_set.
When something is ready, you check which socket it is.
If it’s the server socket → accept a new connection.
If it’s a client socket → read its data and respond.

*** Now replaced by epoll_wait() and event array ***
*/
