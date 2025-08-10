#ifndef CLIENT_H
#define CLIENT_H

#include<string>

class TCPClient{
    public:
        TCPClient(const std::string &server_ip, int port);
        ~TCPClient();

        void start();
    
    private:
        int sock_fd;
        std::string server_ip;
        int port;

        void connectToServer();
        void sendMessage(const std::string &msg);
        void receiveMessage();
};

#endif