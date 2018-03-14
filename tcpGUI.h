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
    struct FIFObuffer *inMessage;
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

inboxQueue *initInboxQueue(void);

int delInboxQueue(inboxQueue *inbox);

int retrieveInboxMessage(char *message, inboxQueue *inbox);

int writeInboxMessage(char *message, inboxQueue *inbox);

int requestRoom(roomList *allRoom, fifo *outputFIFO, char *userName);

// when receiving the confirmation of room from a server, mark the room as ready
int receiveRoom(roomList *allRoom, int serverroomNum);

// close the room and let the server know that the room number is free to be used by others
int closeRoom(chatRoom *room, fifo *outputFIFIO, char *userName);

roomList *roomsetInit(void);

int roomsetDel(roomList *allRoom);

// return a pointer to the room specified by the server room number
chatRoom *retrieveRoom(roomList *allRoom, int roomNum);

// find a room that has been synchronized with the server
chatRoom *findReadyRoom(roomList *allRoom);

// copy the received message to the buffer
int fetchMessage(chatRoom *room, char *buffer);

// copy the user message to the output queue of the program
int sendMessage(chatRoom *room, fifo *outputFIFO, char *userName, char *message);

int sendToServer(fifo *outputFIFO, serverConnection *server);

// get a message from a server and put it in the correct fifo
int recvMessageFromServer(roomList *allRoom, inboxQueue *inbox, serverConnection *server);

int parseCommand(inboxQueue *inbox);

#endif