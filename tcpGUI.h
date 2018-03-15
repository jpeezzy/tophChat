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
    char *friendList[MAX_USER_PER_ROOM];
    int memberChanged; // flag if the member list change since last time
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

serverConnection *openConnection(char *userName, unsigned long int publicKey);

int closeConnection(serverConnection *server);

inboxQueue *initInboxQueue(void);

int delInboxQueue(inboxQueue *inbox);

int retrieveInboxMessage(char *message, inboxQueue *inbox);

int writeInboxMessage(char *message, inboxQueue *inbox);

/******************* ROOM_STUFFS *************************/

// ROOM FINDING
// return a pointer to the room specified by the server room number
chatRoom *retrieveRoom(roomList *allRoom, int roomNum);

// find a room that has been synchronized with the server
chatRoom *findReadyRoom(roomList *allRoom);

// ROOM STARTING ASKING AND ACCEPTING

// request room from server, return the index of the room that is used for requesting
int requestRoom(roomList *allRoom, fifo *outputFIFO, char *userName);

roomList *roomsetInit(void);

// get a message from a server and put it in the correct fifo
int recvMessageFromServer(roomList *allRoom, inboxQueue *inbox, serverConnection *server);

int updateRoomFriendList(chatRoom *room, char **friendList);

int sendToServer(fifo *outputFIFO, serverConnection *server);

int sendAllToServer(fifo *outputBuffer, serverConnection *server);

int parseInboxCommand(inboxQueue *inbox, roomList *roomList, fifo *outputBuffer, char *userName, serverConnection *server);

#endif