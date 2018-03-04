#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
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
#include <assert.h>

#include "tcpGUI.h"
#include "fifo.h"
#include "constants.h"
#include "protocol.h"
#include "tcpPacket.h"
#define DEBUG

serverConnection *openConnection(void)
{
    serverConnection *server = NULL;
    server = (serverConnection *)malloc(sizeof(serverConnection));
    assert(server);

    struct addrinfo *serverInfo = NULL;
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
    getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
#endif
    assert(serverInfo);
    server->socket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    while ((connect(server->socket, serverInfo->ai_addr, serverInfo->ai_addrlen)) == -1 && count < RECONNECT_NUM)
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

int openRoom(chatRoom *room);

int closeRoom(chatRoom *room);

int fetchMessage(chatRoom *room, char *buffer, serverConnection *server)
{
    // TODO: change this to fifo based
    char packet[PACKAGE_SIZE];
    fetchPacket(packet, server->socket);
    strcpy(buffer, packet + CHAT_ROOM_CHARACTER + ID_LENGTH);
    return 0;
}

int sendMessage(chatRoom *room, char *message, serverConnection *server)
{
    char packet[PACKAGE_SIZE];
    char roomNum[CHAT_ROOM_CHARACTER + 1];
    char idString[7]; // identify if it's a command or regular message
    packet[0] = '\0';

    snprintf(roomNum, (CHAT_ROOM_CHARACTER + 1) * sizeof(char), "%d", room->roomNum);
    strcat(packet, roomNum);
    strcat(packet, ID_MESS);
    strcat(packet, message);

    // TODO: change this to mailman thread
    sendPacket(packet, server->socket);
    return 0;
}

//create a function with static identifer