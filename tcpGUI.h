#ifndef _TCP_GUI_H
#define _TCP_GUI_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "constants.h"
#include "fifo.h"

typedef struct serverConnection connection;
typedef struct messageRoom chatRoom;
typedef struct allRoom roomList;
struct serverConnection
{
    // socket used for connection
    int socket;
};

struct room
{
    struct serverConnection server;
    int roomNum;
    struct FIFObuffer inMessage;
};

// head of the linked list connecting all the rooms on the client
struct allRoom
{
    int totalRoom;
    struct room roomList[CHAT_ROOM_LIMIT];
};

int openConnection(connection *server);

int closeConnection(connection *server);

// will need to request a room number from the server
int openRoom(chatRoom *room);

// make available the room number so that the server
// can use it for other room
int closeRoom(chatRoom *room);

// return the next message in the FIFO
char *fetchMessage(chatRoom *room);

// package and send the message both to the input FIFO of the room
// as well as to the output FIFO of the client
int sendMessage(chatRoom *room, char *message);
#endif