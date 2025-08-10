#include "../include/Server.h"

int main() {

    //Creates a TCPServer object configured to listen on TCP port 8080.
    TCPServer server(8080);  
    
    /*
    Calls the server’s main loop, which:
    Sets up the listening socket,
    Waits for incoming connections,
    Handles multiple clients via epoll().
    */
    server.start();
    return 0;
}
