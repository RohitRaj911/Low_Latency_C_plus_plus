/*
Holds:

Listening socket

List of clients

start() method

handleConnections() method (with select())
*/

#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <sys/select.h>

class TCPServer{
    public:
        TCPServer(int port);   //Constructor
        ~TCPServer();   //Desctructor

        void start();

    private:
        int server_fd; //File descriptor for the server's listening socket.
        int port;  // Port number on which the server will listen for connections.
        fd_set master_set;   //A set of file descriptors used by select() to track active sockets.
        int max_fd;   //The highest-numbered file descriptor currently in use (needed for select()).

        void setupServerSocket();   //Initializes and configures the server's listening socket.
        void handleNewConnection();   // Accepts and sets up a newly connected client socket.
        void handleClientMessage(int client_fd);   //Reads and processes incoming messages from a specific client.
        void closeClient(int client_fd);   //Closes the given client socket and removes it from the active set.
};

#endif