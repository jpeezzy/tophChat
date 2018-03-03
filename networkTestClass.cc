#include <iostream>
#include <string>

using namespace std;
extern "C" 
{
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <strings.h>    //bzero
#include <sys/select.h> //io multiplexing

#include "server_back_end.h"
#include "constants.h"
#include "tcpGUI.h"
}


class testServer
{
    private:
        struct sockaddr* addr;
        socklen_t len;
        int serverSocket;
        int newsocket;
    public: 
        testServer(void);
        int sendTestPacket(char array[TEST_PACKET_SIZE]);
        int recvTestPacket(void);
        char receivedPacket[TEST_PACKET_SIZE];
};

testServer :: testServer()
{
    serverSocket=listenSocketInit();
    newsocket= accept(serverSocket, addr, &len);
} 

int testServer :: sendTestPacket(char packet[TEST_PACKET_SIZE])
{
    if(send(newsocket, packet, sizeof(packet),0)<0)
    {
        cout<<"error sending server test messages\n";
        return -1;
    }
    else
    {
        return 0; 
    }
    
    
}

int testServer :: recvTestPacket()
{
    if(recv(newsocket, receivedPacket, sizeof(receivedPacket),0) <0)
    {
        cout<<"error receiving server test messages\n";
        return -1;
    }
    else
    {
        return 0; 
    }
}

class testClient
{
    private:
        serverConnection* server;
         struct addrinfo *serverInfo;
         struct addrinfo serverHints;
    public:
        testClient(void);
        ~testClient(void);
        int sendTestPacket(char array[TEST_PACKET_SIZE]);
        int recvTestPacket(void);
        char receivedPacket[TEST_PACKET_SIZE];
};

testClient :: testClient()
{
    server=(serverConnection*)malloc(sizeof(serverConnection));
    assert(server);
    bzero(&serverHints, sizeof(serverHints));
    serverHints.ai_family = AF_INET;
    serverHints.ai_socktype = SOCK_STREAM;
    getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
    assert(serverInfo);
    server->socket=socket(serverInfo->ai_family, serverInfo->ai_socktype,serverInfo->ai_protocol);
    assert(connect(server->socket, serverInfo->ai_addr, serverInfo->ai_addrlen));   
}

testClient :: ~testClient()
{
    free(server);
    close(server->socket);
    freeaddrinfo(serverInfo);
}

int testClient:: sendTestPacket(char packet[TEST_PACKET_SIZE])
{
    if(send(server->socket, packet, sizeof(packet),0)<0)
    {
        cout<<"error sending server test messages\n";
        return -1;
    }
    else
    {
        return 0; 
    }
}

int testClient:: recvTestPacket(void)
{
    if(recv(server->socket, receivedPacket, sizeof(receivedPacket),0) <0)
    {
        cout<<"error receiving server test messages\n";
        return -1;
    }
    else
    {
        return 0; 
    }
}