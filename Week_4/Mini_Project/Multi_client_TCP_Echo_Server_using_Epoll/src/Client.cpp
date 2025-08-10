/*
3-way HandShake

let’s peek behind the curtain and see what really happens when connect()
When your code runs:

connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
The OS kicks off the TCP three-way handshake to establish a reliable connection:

1️⃣ SYN (Synchronize) — Client says “Hello”
Your client sends a SYN packet to the server with an initial sequence number (like saying “Let’s start talking, and here’s my message counter”).

This tells the server:
“I want to start a TCP conversation, and here’s where my counting begins.”

2️⃣ SYN-ACK (Synchronize + Acknowledge) — Server says “Hello back”
If the server is listening and accepts connections on that port:

It replies with SYN-ACK:
SYN: “Okay, I’ll talk to you. Here’s my sequence number.”
ACK: “And I confirm I got your sequence number.”


3️⃣ ACK (Acknowledge) — Client confirms
Your client sends back an ACK:
“Got your sequence number, let’s start sending data.”

✅ At this point, both sides know each other’s sequence numbers and
 the TCP connection is officially established. Now connect() returns, 
 and you can send() and recv() data.

*/


#include "../include/Client.h"
#include "../include/Utils.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

#define BUFFER_SIZE 1024

TCPClient :: TCPClient(const std::string &ip, int port) 
: server_ip(ip), port(port), sock_fd(-1) {};

TCPClient::~TCPClient() {
    if (sock_fd != -1) close(sock_fd);
}


void TCPClient :: connectToServer(){
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) Utils::errorExit("Socket failed");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    //Converts the server IP string (e.g., "127.0.0.1") into a binary form 
    //and stores it in server_addr.sin_addr; exits if invalid.
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0)
        Utils::errorExit("Invalid address");

    //Attempts to connect to the server using the socket 
    //and the address struct; exits if the connection fails.
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        if (errno == ECONNREFUSED) {
            std::cerr << "🚫 Server is down, please wait...\n";
        }
        Utils::errorExit("Connection failed");
    }

    // ✅ Set a 5-second receive timeout
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt failed");
    }

    std::cout << "✅ Connected to server at " << server_ip << ":" << port << "\n";
}


/*
Purpose: Sends a message to the connected server.
msg.c_str() → Converts the std::string to a C-style string (const char*).
msg.size() → Gets the number of bytes to send.
send(..., 0) → Sends the data over TCP without special flags.
*/
void TCPClient::sendMessage(const std::string &msg) {
    send(sock_fd, msg.c_str(), msg.size(), 0);
}

void TCPClient::receiveMessage() {
    char buffer[BUFFER_SIZE] = {0};  //Creates a buffer filled with zeros to store incoming data.

    //Waits for data from the server and stores it in buffer.
    //Returns the number of bytes actually received (bytes_received).
    ssize_t bytes_received = recv(sock_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        std::cout << "📩 Server: " << buffer;
    }
    else if (bytes_received == 0) {
        std::cout << "⚠️ Server closed the connection.\n";
    }
    else {

        /*
        Checks the global errno variable to see why recv() failed:
        EWOULDBLOCK / EAGAIN → These mean “no data is available right now” on a non-blocking socket.
        This is not a fatal error — it just means “try again later.”
        */

        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            std::cout << "⏳ Server is not responding, please wait...\n";
        } else {
            perror("recv");  //If the error wasn’t “no data yet,” it’s a real problem — perror("recv") prints the error reason.
        }
    }
}


void TCPClient::start() {
    connectToServer();  //Establishes a TCP connection to the server (performs the handshake we discussed earlier).

    std::string msg;

    //Starts an infinite loop so the client can continuously send and receive messages
    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, msg);
        if (msg == "quit") break;

        sendMessage(msg + "\n");  //Sends the message to the server, adding a newline at the end.
        receiveMessage();  //Waits for and prints the server’s reply (in this case, an echo).
    }
}

//In SHort: Connect → Loop → Prompt → Send → Receive → Repeat until "quit".