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
typedef struct messageServerRoom serverChatRoom;
typedef struct allServerRoom serverRoomList;

struct messageServerRoom
{
    int adminID;
    int roomNum;
    struct FIFObuffer inMessage;
    struct FIFObuffer outMessage;
    int socketList[MAX_USER_PER_ROOM];
};

// head of the linked list connecting all the rooms on the client
struct allServerRoom
{
    int totalRoom;
    struct messageRoom roomList[MAX_SERVER_CHATROOMS];
};

// the
typedef int (*roomFunc)(struct messageServerRoom *room, int num, char *);
typedef int (*friendFunc)(char *name1, char *name2);
typedef int (*comFunc)(int socket);

int listenSocketInit(void);

int serverRoomCreate(struct messageServerRoom *room);

int serverRoomDel(struct messageServerRoom *room);

int serverRoomFIFOWrite(struct messageServerRoom *room);

#endif