#include <socket.h>
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

// #include <pthread.h>

#include "constants.h"
#include "tcpGUI.h"

int spawnChildThread()
{
}

// return an available room number
int assignRoom(roomList *);

int main(void)
{

    fd_set *setListener;
    FD_ZERO(setListener);
    struct timeval timeoutListener;
    timeoutListener.tv_sec = LISTENER_TIMEOUT;

    fd_set *setClient;
    FD_ZERO(setClient);
    struct timeval timeoutClient;
    timeoutListener.tv_sec = CLIENT_TIMEOUT;

    // the socket that will be listening
    struct addrinfo *serverListener;
    int socketListener;
    // give the server hints about the type of connection we want
    struct addrinfo serverHints;
    serverHints.ai_family = AF_INET;       // IPV4 address
    serverHints.ai_socktype = SOCK_STREAM; // TCP socket
    serverHints.ai_flags = AI_PASSIVE;     // listening socket

    // create a socket for listening incoming collection
    getaddrinfo(NULL, CHAT_SERVER_PORT, &serverHints, &serverListener);
    socketListener = socket(serverListener->ai_family, serverListener->ai_socktype, 0);
    bind(socketListener, serverListener->ai_addr, serverListener->ai_addrlen);
    listen(socketListener, MAX_SERVER_USERS);
    FD_SET(socketListener, setListener);

    // forever loop, main thread watches for new client, other thread handles one client
    for (;;)
    {
    }
    return 0;
}