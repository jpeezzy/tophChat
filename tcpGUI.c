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
#include "utils.h"
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

// copy the received message to the buffer
int fetchMessage(chatRoom *room, char *buffer, serverConnection *server)
{
    readBuffer(&(room->inMessage), buffer);
    return 0;
}

// used for sending both command and regular messages
int sendMessage(chatRoom *room, char *message, serverConnection *server, int isCommand)
{
    char packet[PACKAGE_SIZE];
    char roomNum[CHAT_ROOM_CHARACTER + 1];
    packet[0] = '\0';

    snprintf(roomNum, (CHAT_ROOM_CHARACTER + 1) * sizeof(char), "%d", room->roomNum);
    strcat(packet, roomNum);
    if (isCommand)
    {
        strcat(packet, ID_COMM);
    }
    else
    {
        strcat(packet, ID_MESS);
    }
    strcat(packet, message);

    // TODO: change this to mailman thread
    sendPacket(packet, server->socket);
    return 0;
}

chatRoom *requestRoom(roomList *allRoom, serverConnection *server)
{
    int i = 0;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList[i]).status == 2)
        {
            return &(allRoom->roomList[i]);
        }
    }
    if (i == allRoom->totalRoom)
    {
        return NULL;
    }
}

int closeRoom(chatRoom *room, serverConnection *server)
{
    char commandString[PACKAGE_SIZE] = "R2";
    room->status = 0; // freed
    sendMessage(room, commandString, server, 1);
    return 0;
}

int allocateRoom(roomList *allRoom, serverConnection *server)
{
    int i = 0;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList[i]).status == 0)
        {
            char commandString[PACKAGE_SIZE] = "R5";
            (allRoom->roomList[i]).status = 1; // requested
            sendMessage(&(allRoom->roomList[i]), commandString, server, 1);
        }
    }
}

int mailMan(roomList *allRoom, serverConnection *server)
{
    char packet[PACKAGE_SIZE];
    char packetType[ID_LENGTH+1];
    char roomNum[CHAT_ROOM_CHARACTER+1];
    if (fetchPacket(packet, server->socket) != SOCKET_NO_DATA)
    {
        stringSlicer(packet, messageType, CHAT_ROOM_CHARACTER, CHAT_ROOM_CHARACTER + ID_LENGTH - 1);
        if (strcmp(messageType, ID_MESS) == 0)
        {
            stringSlicer()
        }
    }

    // no data
    return -1;
}
//create a function with static identifer