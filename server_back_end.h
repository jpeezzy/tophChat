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
#include "tophChatUsers.h"
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

// update which room is not occupied
void updateFreeRoom(struct allServerRoom *allRoom);

struct allServerRoom *serverRoomSetCreate(void);

// mark the room as free when conversation is done
void serverRoomReturn(serverChatRoom *room);

// send message to everyone in the room except the writer
int serverRoomSpreadMessage(struct messageServerRoom *room, TINFO *dataBase);

// put the user message onto the server room fifo
int sendServerRoomMessage(struct messageServerRoom *room, char *userPacket);

// /*******************************USER STUFFS HERE**************************/
// create a list of online user
onlineUserList *serverCreateOnlineList(void);

void serverDelOnlineList(onlineUserList *allOnlineUser);

// add user to the list of online user
onlineUser *serverAddOnlineUser(char *userName, onlineUserList *allUser, int socket, TINFO *database);

int serverLogOffUser(onlineUser *user);

int sendRoomInvite(char *userNameRequester, char *userNameTarget, serverChatRoom *room, TINFO *database);

int addUserToServerRoom(serverChatRoom *room, char *userNameTarget, TINFO *dataBase);

int removeUserFromServerRoom(serverChatRoom *room, char *userNameTarget, TINFO *dataBase);

serverChatRoom *findServerRoomByNumber(struct allServerRoom *allRoom, int roomNum);

int getFriendList(TUSER *user);

// get a list of online user separated by '/'
int getOnlineUser(onlineUserList *allUsers, char *onlineList);

int triagePacket(onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase);
#endif