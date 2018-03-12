#ifndef _SERVER_MAIN_H
#define _SERVER_MAIN_H

#include <sys/socket.h>
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

#include "constants.h"
#include "fifo.h"
#include "users/tophChatUsers.h"
typedef struct messageServerRoom serverChatRoom;
typedef struct allServerRoom serverRoomList;
typedef struct allOnlineUser onlineUserList;
typedef struct onlUser onlineUser;
// when a message is received by the server, it will be parsed to include the original, room and who sent them

// head of the linked list connecting all the rooms on the client
struct messageServerRoom
{
    int adminID;
    int isOccupied; //0 for free 1 for occupied
    int roomNum;
    int peopleNum;
    struct FIFObuffer *inMessage;
    int socketList[MAX_USER_PER_ROOM];
    struct onlUser *userList[MAX_USER_PER_ROOM];
};

struct allServerRoom
{
    int totalRoom;
    int firstFreeRoom;
    struct messageServerRoom roomList[MAX_SERVER_CHATROOMS];
};

// list of all users currently online

struct onlUser
{
    struct tophChatUser *userProfile; //pointer to the main database
    int slot_status;
    int roomList[CHAT_ROOM_LIMIT];
    int totalRoomIn;
};

struct allOnlineUser
{
    struct onlUser userList[MAX_SERVER_USERS];
    int totalOnlineUser;
};

typedef int (*roomFunc)(struct messageServerRoom *room, int num, char *);
typedef int (*friendFunc)(char *name1, char *name2);
typedef int (*comFunc)(int socket);

int listenSocketInit(void);

struct messageServerRoom *serverRoomCreate(struct allServerRoom *allRoom);

int serverRoomDel(struct messageServerRoom *room);

int serverRoomFIFOWrite(struct messageServerRoom *room);

#endif