#include "tcpGUI.h"
#include "constants.h"

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
