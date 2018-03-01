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

#define TEST_SERVER
int listenSocketInit(void)
{
    struct timeval timeoutListener;
    timeoutListener.tv_sec = LISTENER_TIMEOUT;

    struct timeval timeoutClient;
    timeoutListener.tv_sec = CLIENT_TIMEOUT;

    // the socket that will be listening
    struct addrinfo *serverListener;
    int socketListener;
    // give the server hints about the type of connection we want
    struct addrinfo serverHints;
    memset(&serverHints, 0, sizeof(serverHints));
    serverHints.ai_family = AF_INET;       // IPV4 address
    serverHints.ai_socktype = SOCK_STREAM; // TCP socket
    serverHints.ai_flags = AI_PASSIVE;     // listening socket

    // create a socket for listening incoming collection
    getaddrinfo(NULL, CHAT_SERVER_PORT, &serverHints, &serverListener);
    socketListener = socket(serverListener->ai_family, serverListener->ai_socktype, 0);
    if(socketListener<0)
    {
        return SOCKET_NO_CONNECTION;
         freeaddrinfo(serverListener);
    }
    else
    {
        bind(socketListener, serverListener->ai_addr, serverListener->ai_addrlen);
        listen(socketListener, MAX_SERVER_USERS);
         freeaddrinfo(serverListener);
        return socketListener;
    }
}

#ifdef TEST_SERVER
int main(void)
{
    struct sockaddr* addr;
    socklen_t len;
    char dummyPacket[]="123";
    char receivedPacket[100];
    int socket=listenSocketInit();
    int newsocket= accept(socket, addr, &len);
    //recv(newsocket, receivedPacket, sizeof(receivedPacket),0);
    //printf("%s", receivedPacket);
    send(newsocket, dummyPacket, sizeof(dummyPacket),0);
    return 0;
}
#endif