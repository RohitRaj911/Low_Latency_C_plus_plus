#include "../include/Server.h"
#include "../include/Utils.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

#define BUFFER_SIZE 1024

/*
This is the member initializer list — it sets class members before the constructor body runs:
port(port) → Assigns the passed argument port to the member variable port.
server_fd(-1) → Initializes server_fd to -1 (meaning “no valid socket yet”).\
max_fd(0) → Initializes max_fd to 0 (no sockets tracked yet).
*/
TCPServer :: TCPServer(int port) : port(port), server_fd(-1), max_fd(0) {
    FD_ZERO(&master_set);
}

TCPServer::~TCPServer() {
    if (server_fd != -1) close(server_fd);
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

    FD_SET(server_fd, &master_set);  //Adds the server’s listening socket to master_set so select() can monitor it for new connections.
    max_fd = server_fd;  //Sets max_fd to the server’s FD so select() knows the range of file descriptors to check.

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

    FD_SET(client_fd, &master_set);  //Adds the new client’s socket descriptor to master_set so select() can monitor it for incoming data.
    
    // Updates max_fd if the new client’s descriptor number is the highest seen so far (needed for select() efficiency).
    if (client_fd > max_fd) max_fd = client_fd;

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
    FD_CLR(client_fd, &master_set);
}


void TCPServer::start() {
    // Initializes the server socket, binds it to the port, starts listening, and adds it to master_set.
    setupServerSocket();


    // Enters the main infinite event loop that continuously checks for socket activity.
    while (true) {
        fd_set read_fds = master_set; //Makes a copy of master_set because select() modifies the set passed to it, and we need the original for future loops.   

        // Waits for activity on any monitored socket (read events only here), blocking indefinitely until something happens.
        int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);

        //If select() fails, print the error and exit the loop.
        if (activity < 0) { 
            perror("Select error");
            break;
        }


        //Loops through all possible file descriptors up to the current maximum (max_fd).
        for (int fd = 0; fd <= max_fd; fd++) {
            //Checks if this specific file descriptor is marked as ready to read by select().
            if (FD_ISSET(fd, &read_fds)) {

                /*
                If the ready socket is the server socket → accept a new client;
                otherwise → read data from an existing client and process it.
                */
                if (fd == server_fd) {
                    handleNewConnection();
                } else {
                    handleClientMessage(fd);
                }
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
*/