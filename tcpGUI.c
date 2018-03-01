/**
 * List of API used in GUI code 
 */
#include <sys/socket.h>
#include <assert.h>

#include "tcpGUI.h"
#include "constants.h"

serverConnection* openConnection(void)
{
    serverConnection* server=(serverConnection*)malloc(sizeof(serverConnection));
    assert(server);

    struct addrinfo *serverInfo;
    struct addrinfo serverHints;

    bzero(&serverHints, sizeof(serverHints));

    int count = 0;

    // give hints about TCP connection and IPV4
    serverHints.ai_family = AF_INET;
    serverHints.ai_socktype = SOCK_STREAM;

    #ifndef DEBUG
    // assuming that the chat server is on zuma
    getaddrinfo(CHAT_SERVER_ADDR, CHAT_SERVER_PORT, &serverHints, &serverInfo);
    #else
    serverHints.ai_flags = serverHints.ai_socktype = SOCK_STREAM;;
    getaddrinfo(NULL, CHAT_SERVER_PORT, &serverHints, &serverInfo);
    #endif

    while ((connect(server->socket, serverInfo->ai_addr, &(serverInfo->ai_addrlen))) == -1 && count < RECONNECT_NUM)
    {
        ++count;
    }

    // can't connect to server
    if (count == 5)
    {
        // return SOCKET_CANT_CONNECT;
        return NULL;
    }

    freeaddrinfo(serverInfo);
    return server;
}

int closeConnection(serverConnection *server)
{
    close(server->socket);
    free(server);
    return 0;
}
