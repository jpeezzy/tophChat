#include <iostream>
#include <string>
#include "networkTestClass.h"

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

// SERVER CLASS USED FOR TESTING 
void testServer :: setupServer()
{
    assert((serverSocket=listenSocketInit())>=0);
    newsocket= accept(serverSocket, addr, &len)>=0;
    printf("\nnewsocket server error: %s\n",strerror(errno));
}
int testServer :: sendTestPacket(char* packet)
{
    int count=0;
    for(;;)
    {
    if(send(newsocket, packet, PACKAGE_SIZE*sizeof(char),0)<0)
    {
        cout<<"error sending server test messages\n";
        printf("error from server sending:%s\n", strerror(errno));
        if(count >=5)
        {
            return -1;
        }
        else
        {
            ++count;
        }
    }
    else
    {
        return 0; 
    }
    }
    
}

int testServer :: recvTestPacket()
{
    if(recv(newsocket, receivedPacket, PACKAGE_SIZE*sizeof(char),0) <0)
    {
        cout<<"error receiving server test messages\n";
        return -1;
    }
    else
    {
        return 0; 
    }
}

testServer :: ~testServer()
{
    close(newsocket);
    close(serverSocket);
}

// CLIENT CLASS USED FOR TESTING
void testClient :: setupClient()
{
    server=(serverConnection*)malloc(sizeof(serverConnection));
    assert(server);
    bzero(&serverHints, sizeof(serverHints));
    serverHints.ai_family = AF_INET;
    serverHints.ai_socktype = SOCK_STREAM;
    getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
    assert(serverInfo);
    server->socket=socket(serverInfo->ai_family, serverInfo->ai_socktype,serverInfo->ai_protocol);
    assert(connect(server->socket, serverInfo->ai_addr, serverInfo->ai_addrlen)>=0);   
}

testClient :: ~testClient()
{
    free(server);
    close(server->socket);
    freeaddrinfo(serverInfo);
}

int testClient:: sendTestPacket(char packet[TEST_PACKET_SIZE])
{
    if(send(server->socket, packet, PACKAGE_SIZE*sizeof(char),0)<0)
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
    if(recv(server->socket, receivedPacket, PACKAGE_SIZE*sizeof(char),0) <0)
    {
        cout<<"error receiving server test messages\n";
        return -1;
    }
    else
    {
        return 0; 
    }
}