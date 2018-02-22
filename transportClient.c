#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>


#include "transportClient.h"

#define CHAT_SERVER_ADDR "zuma.eecs.uci.edu"
#define RECONNECT_NUM 5
#define CHAT_SERVER_PORT "30001"

int openConnection(struct serverConnection* server)
{
    struct addrinfo* serverInfo;
    struct addrinfo serverHints;
    
    bzero(&serverHints, sizeof(serverHints));

    int count=0;

    // give hints about TCP connection and IPV4
    serverHints.ai_family=AF_INET;
    serverHints.ai_socktype=SOCK_STREAM;

    // assuming that the chat server is on zuma  
    getaddrinfo(CHAT_SERVER_ADDR, CHAT_SERVER_PORT, &serverHints, &serverInfo);

    while((connect(server->socket, serverInfo->ai_addr, &(serverInfo->ai_addrlen)))==-1 && count <RECONNECT_NUM)
    {   
        ++count;
    }

    // can't connect to server
    if(count==5)
    {
        return -1;
    }

    // check if failed to bind 
    freeaddrinfo(serverInfo);
    return 0;
}


int closeConnection(struct serverConnection* server)
{
    return close(server->socket);
}


int sendPacket(clientPacket* packet, struct serverConnection* server)
{
    clientPacket* curByte=packet; 
    int packetLeft=sizeof(packet);
    int sent;

    //implement check to make it sends all bytes
    while(packetLeft>0)
    {   

        sent=send(server->socket, (void*) curByte, sizeof(packet),0);
        if(sent==-1)
        {
            if(errno==EPIPE)
            {
                // the server socket has closed 
                return -2;
            }
        }
        packetLeft-=sent;
        curByte+=sent;
    }
    return 0; 
}

int fetchPacket(clientPacket* packet, struct serverConnection* server)
{
    int temp=recv(server->socket, packet, sizeof(packet), MSG_DONTWAIT);
    if(temp==-1)
    {
        if(errno==EWOULDBLOCK)
        {   
            //no data to read
            return -1;
        }
    }

    else if(temp==0)
    {
        // the server socket has closed 
        return -2;
    }

    return 0;
}

int main(void)
{   

    return 0;
}
