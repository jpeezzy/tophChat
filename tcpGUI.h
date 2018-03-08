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

typedef struct sConnection serverConnection;
typedef struct messageRoom chatRoom;
typedef struct allRoom roomList;
typedef struct requestStorage inboxQueue;
// structure used for establishing the initial connection
// have cookies to password saving
struct sConnection
{
    // socket used for connection
    int socket;
    FILE *cookies_file; // file that stores a private key and undefined number of public key
    int privKey;
    int *pubKey;
    int pubKeyLength;
};

struct messageRoom
{
    int roomNum; // this is the room number from the server
    int status;
    int offSet; //offset from local room number and server room
    struct FIFObuffer *inMessage;
    struct FIFObuffer *outMessage;
};

// head of the linked list connecting all the rooms on the client
struct allRoom
{
    int totalRoom;
    int totalAllocatedRoom; // room that has been confirmed with the server
    struct messageRoom roomList[CHAT_ROOM_LIMIT];
};

// list carrying special messages like friend request
struct requestStorage
{
    fifo *messageQueue;
};

serverConnection *openConnection(void);

int closeConnection(serverConnection *server);

// will need to request a room number from the server
int openRoom(chatRoom *room);

// make available the room number so that the server
// can use it for other room
int closeRoom(chatRoom *room, serverConnection *server);

// return the next message in the FIFO
int fetchMessage(chatRoom *room, char *buffer);

// package and send the message both to the input FIFO of the room
// as well as to the output FIFO of the client
int sendMessage(chatRoom *room, char *message, serverConnection *server);
#endif