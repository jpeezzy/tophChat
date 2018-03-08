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
#include "protocol_const.h"
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
        return -1;
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

inboxQueue *initInboxQueue(void)
{
    inboxQueue *inbox = malloc(sizeof(inboxQueue));
    inbox->messageQueue = initBuffer(MAX_REQUEST);
    return inbox;
}

int delInboxQueue(inboxQueue *inbox)
{
    closeBuffer(inbox->messageQueue);
    free(inbox);
    return 0;
}

int retrieveInboxMessage(char *message, inboxQueue *inbox)
{
    readBuffer(inbox->messageQueue, message);
    return 0;
}

int writeInboxMessage(char *message, inboxQueue *inbox)
{
    writeBuffer(inbox->messageQueue, message);
    return 0;
}

int requestRoom(roomList *allRoom, fifo *outputFIFO)
{
    // TODO: thread sensitive
    int i = 0;
    char tempMessage[PACKAGE_SIZE];
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList[i]).status == 0)
        {
            assembleCommand(111, ROID, ROCREATE, NULL, tempMessage);
            writeBuffer(outputFIFO, tempMessage);
        }
    }
    return 0;
}

// when receiving the confirmation of room from a server, mark the room as ready
int receiveRoom(roomList *allRoom, int serverroomNum)
{
    int i = 0;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList)[i].status == ROOM_WAITING)
        {
            (allRoom->roomList)[i].roomNum = serverroomNum;
            (allRoom->roomList)[i].status = ROOM_READY;
            break;
        }
    }
    // TODO: implement error response for no waiting rooms
    if (i == allRoom->totalRoom)
    {
        return NO_WAITING_ROOM;
    }
}

// close the room and let the server know that the room number is free to be used by others
int closeRoom(chatRoom *room, fifo *outputFIFIO)
{
    char tempPacket[PACKAGE_SIZE] = "";
    room->status = 0; // freed
    assembleCommand(room->roomNum, ROID, RODEL, NULL, tempPacket);
    writeBuffer(outputFIFIO, tempPacket);
    return 0;
}

roomList *roomsetInit(void)
{
    roomList *temproomSet = malloc(sizeof(struct allRoom));
    temproomSet->totalRoom = CHAT_ROOM_LIMIT;
    for (int i = 0; i < temproomSet->totalRoom; ++i)
    {
        temproomSet->roomList[i].status = ROOM_UNALLOCATED;
        temproomSet->roomList[i].inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
    }
    return temproomSet;
}

int roomsetDel(roomList *allRoom)
{
    free(allRoom);
    return 0;
}

// return a pointer to the room specified by the server room number
chatRoom *retrieveRoom(roomList *allRoom, int roomNum)
{
    int i = 0;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList)[i].roomNum == roomNum)
        {
            return &((allRoom->roomList)[i]);
        }
    }
    if (i == allRoom->totalRoom)
    {
        return NULL;
    }
}

// find a room that has been synchronized with the server
chatRoom *findReadyRoom(roomList *allRoom)
{
    int i = 0;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList[i]).status == ROOM_READY)
        {
            return &(allRoom->roomList[i]);
        }
    }
    if (i == allRoom->totalRoom)
    {
        return NULL;
    }
}

// copy the received message to the buffer
int fetchMessage(chatRoom *room, char *buffer)
{
    // TODO: implement muxtex if using multithreaded
    readBuffer(room->inMessage, buffer);
    return 0;
}

// copy the user message to the output queue of the program
int sendMessage(chatRoom *room, fifo *outputFIFO, char *message)
{
    assert(message);
    char tempPacket[PACKAGE_SIZE];
    // TODO: multithreaded mutex
    assembleMessage(room->roomNum, message, tempPacket);
    writeBuffer(outputFIFO, tempPacket);
    return 0;
}

// int mailMan(roomList *allRoom, serverConnection *server)
// {
//     char packet[PACKAGE_SIZE];
//     chatRoom *tempRoom;
//     char messageBody[MESS_LIMIT + 1];
//     char roomNum[CHAT_ROOM_CHARACTER + 1];
//     if (fetchPacket(packet, server->socket) != SOCKET_NO_DATA)
//     {

//         if (getpacketType(packet) == ISMESSAGE)
//         {
//             // TODO: fix room number mechanism
//             getMessageBody(packet, messageBody);
//             tempRoom = retrieveRoom(allRoom, getroomNumber(packet));
//             writeBuffer(&(tempRoom->inMessage), messageBody, MESS_LIMIT + 1);
//         }
//         else if (getpacketType(packet) == ISCOMM)
//         {
//             switch (getCommandType(packet))
//             {
//             case FRIENDID:
//                 switch (getCommandID(packet))
//                 {
//                 case ROCREATE:
//                     prepareRoom(allRoom, getroomNumber(packet));
//                     break;
//                 case RODEL:
//                     closeRoom(retrieveRoom(allRoom, getroomNumber(packet)), server);
//                 }
//                 break;
//             case ROID:
//                 break;
//             case COMID:
//                 break;
//             default:
//                 return UNKNOWN_COMMAND_TYPE;
//             }
//         }
//         return 0;
//     }
//     // no data
//     return -1;
// }
