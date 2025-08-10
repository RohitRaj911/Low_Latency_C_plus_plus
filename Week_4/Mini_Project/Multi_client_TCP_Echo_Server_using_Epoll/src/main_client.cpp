#include "../include/Client.h"

int main() {
    /*
    Creates a TCP client object configured to connect to:
    IP: 127.0.0.1 (localhost — same machine)
    Port: 8080
    */
    TCPClient client("127.0.0.1", 8080);

    /*
    Runs the client loop:
    Connects to the server (does the TCP handshake),
    Prompts you for messages,
    Sends them to the server,
    Receives and prints the server’s replies,
    Repeats until you type "quit".
    */
    client.start();
    return 0;
}