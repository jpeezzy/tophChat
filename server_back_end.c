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
#include <errno.h>

#include "server_back_end.h"
#include "constants.h"
#include "testString.h"
#include "tcpPacket.h"
#include "users/tophChatUsers.h"
#include "protocol_const.h"
#include "protocol.h"
#include "utils.h"

//#define TEST_SERVER

/**
 * as of right now, the server is single threaded, the server will need to call
 * update free room occassionally 
 */
int listenSocketInit(void)
{
    struct timeval timeoutListener;
    timeoutListener.tv_sec = LISTENER_TIMEOUT;

    struct timeval timeoutClient;
    timeoutListener.tv_sec = CLIENT_TIMEOUT;

    // the socket that will be listening
    struct addrinfo *serverListener;
    int socketListener;

    // give the server hints about the type of connection we want
    struct addrinfo serverHints;
    memset(&serverHints, 0, sizeof(serverHints));
    serverHints.ai_family = AF_INET;       // IPV4 address
    serverHints.ai_socktype = SOCK_STREAM; // TCP socket
    serverHints.ai_flags = AI_PASSIVE;     // listening socket
    int n = 1;

    // create a socket for listening incoming collection
    getaddrinfo(NULL, CHAT_SERVER_PORT, &serverHints, &serverListener);
    socketListener = socket(serverListener->ai_family, serverListener->ai_socktype, serverListener->ai_protocol);
    setsockopt(socketListener, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));
    if (socketListener < 0)
    {
        freeaddrinfo(serverListener);
        return SOCKET_NO_CONNECTION;
    }
    else
    {
        if (bind(socketListener, serverListener->ai_addr, serverListener->ai_addrlen) == -1)
        {
            printf("The error is %s\n", strerror(errno));
        }
        assert(listen(socketListener, MAX_SERVER_USERS) >= 0);
        freeaddrinfo(serverListener);
        return socketListener;
    }
}

/**********************SERVER ROOM STUFFS HERE********************/
// create list of all rooms on server
struct allServerRoom *serverRoomSetCreate(void)
{
    struct allServerRoom *allRoom = malloc(sizeof(struct allServerRoom));
    allRoom->firstFreeRoom = 0;
    allRoom->totalRoom = MAX_SERVER_CHATROOMS;

    for (int i = 0; i < allRoom->totalRoom; ++i)
    {
        ((allRoom->roomList)[i]).isOccupied = ROOM_NOT_OCCUPIED;
        ((allRoom->roomList)[i]).roomNum = i;
        ((allRoom->roomList)[i]).peopleNum = 0;
        ((allRoom->roomList)[i]).inMessage = initBuffer(SERVER_CHAT_ROOM_INPUT_FIFO_MAX);
        for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
        {
            ((allRoom->roomList[i]).userList[j]) = NULL; // will be filled with pointer to the Justin database
        }
    }
}
void serverRoomSetDel(struct allServerRoom *allRoom)
{
    for (int i = 0; i < allRoom->totalRoom; ++i)
    {
        closeBuffer(((allRoom->roomList)[i]).inMessage);
        for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
        {
            (allRoom->roomList[i]).userList[j] = NULL;
        }
    }
    free(allRoom);
}

// update which room is not occupied
void updateFreeRoom(struct allServerRoom *allRoom)
{
    assert(allRoom);
    int i;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((((allRoom->roomList)[i]).isOccupied) == ROOM_NOT_OCCUPIED)
        {
            allRoom->firstFreeRoom = i;
            break;
        }
    }
    if (i == allRoom->totalRoom)
    {
        allRoom->firstFreeRoom = -1;
    }
}

// return a free room
struct messageServerRoom *serverRoomCreate(struct allServerRoom *allRoom)
{
    assert(allRoom);
    updateFreeRoom(allRoom);
    if (allRoom->firstFreeRoom == -1)
    {
        return NULL;
    }
    else
    {
        allRoom->roomList[allRoom->firstFreeRoom].isOccupied = ROOM_BUSY;
        return &(allRoom->roomList[allRoom->firstFreeRoom]);
    }
}

// mark the room as free when conversation is done
void serverRoomReturn(serverChatRoom *room)
{
    assert(room);
    room->isOccupied = ROOM_NOT_OCCUPIED;
    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        room->socketList[i] = -1;
    }

    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        room->userList[i] = NULL;
    }
    room->peopleNum = 0;
}

// send message to everyone in the room except the writer
int serverRoomSpreadMessage(struct messageServerRoom *room, char *serverPacket, TINFO *dataBase)
{
    assert(serverPacket);
    char userName[50];
    TUSER *tempUser;
    assert(room);
    getSenderName(userName, serverPacket);
    tempUser = findUserByName(userName, dataBase);
    if (tempUser == NULL)
    {
        return -1;
    }
    for (int i = 0; i < room->peopleNum; ++i)
    {
        if (room->socketList[i] >= 0)
        {
            if ((room->socketList[i]) != tempUser->socket)
            {
                // TODO: Change this
                assert(sendPacket(serverPacket, room->socketList[i]) >= 0);
            }
        }
    }
}

// put the user message onto the server room fifo
int sendServerRoomMessage(struct messageServerRoom *room, char *userPacket)
{
    // TODO: not thread safe
    return writeBuffer(room->inMessage, userPacket);
}

int addUserToServerRoom(onlineUser *user, serverChatRoom *room)
{
    assert(user->userProfile->socket >= 0);
    if (room->peopleNum == MAX_USER_PER_ROOM)
    {
        return ROOM_FULL;
    }
    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        if (room->socketList[i] == -1)
        {
            room->socketList[i] = user->userProfile->socket;
            break;
        }
    }

    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        if (room->userList[i] == NULL)
        {
            room->userList[i] = user;
        }
    }
    ++(room->peopleNum);
}

// /*******************************USER STUFFS HERE**************************/
// create a list of online user
onlineUserList *serverCreateOnlineList(void)
{
    onlineUserList *userList = malloc(sizeof(onlineUserList));
    userList->totalOnlineUser = 0;
    for (int i = 0; i < MAX_SERVER_USERS; ++i)
    {
        userList->userList[i].slot_status = NOT_ONLINE;
    }
    return userList;
}

void serverDelOnlineList(onlineUserList *allOnlineUser)
{
    free(allOnlineUser);
}

// TODO: replace this with Justin's data structure
// check if the user already exists
onlineUser *serverAddOnlineUser(char *userName, int userSocket, onlineUserList *allUser, TINFO *database)
{
    int i;
    if (allUser->totalOnlineUser == MAX_SERVER_USERS)
    {
        // server full
        return NULL;
    }
    TUSER *tempUser = findUserByName(userName, database);

    // user already online
    if (tempUser->socket != NOT_ONLINE)
    {
        return NULL;
    }
    for (i = 0; i < MAX_SERVER_USERS; ++i)
    {
        if (allUser->userList[i].slot_status = NOT_ONLINE)
        {
            allUser->userList[i].slot_status = ONLINE;
            allUser->userList[i].userProfile = tempUser;
            return &(allUser->userList[i]);
        }
    }
}

int serverLogOffUser(onlineUser *user)
{
    // TODO: use Justin functions
    close(user->userProfile->socket);
    user->slot_status = NOT_ONLINE;
    user->userProfile->socket = NOT_ONLINE;
    return 0;
}

int sendRoomInvite(char *userNameRequester, char *userNameTarget, chatRoom *room, onlineUserList *allUser, TINFO *database)
{
    int i = 0;
    char packet[PACKAGE_SIZE];
    TUSER *target = findUserByName(userNameTarget, database);
    TUSER *sender = findUserByName(userNameRequester, database);
    if (target == NULL || sender == NULL)
    {
        return USER_NOT_EXIST;
    }

    if (!checkIfFriends(target, sender))
    {
        return USER_NOT_FRIEND;
    }

    if (target->socket == NOT_ONLINE)
    {
        return USER_NOT_ONLINE;
    }

    assembleCommand(room->roomNum, ROID, ROINVITE, NULL, packet);
    sendPacket(packet, allUser->userList[i].userProfile->socket);
    return 0;
}

// /********************************UTILITIES HERE*********************************/
// // TODO implement lookup
// int lookUpUser();

// return the pointer to the room based on room number
serverChatRoom *lookUpRoom(struct allServerRoom *allRoom, int roomNum)
{
    int j = 0;
    for (j = 0; j < allRoom->totalRoom; ++j)
    {
        if (allRoom->roomList[j].roomNum == roomNum)
        {
            return &(allRoom->roomList[j]);
        }
    }
    return NULL;
}

    // // TODO: make this multithreaded
    // // do all administrative tasks to keep server running
    // int serverMailman(onlineUserList *userList, struct allServerRoom *allRoom)
    // {
    //     char packet[PACKAGE_SIZE];
    //     struct messageServerRoom *tempRoom;
    //     char messageBody[MESS_LIMIT + 1];
    //     int roomNum;
    //     struct messageServerRoom *tempRoom;
    //     for (int i = 0; i < userList->totalUser; ++i)
    //     {
    //         if (fetchPacket(packet, userList->userList[i].socket) != SOCKET_NO_DATA)
    //         {
    //             if (getpacketType(packet) == ISMESSAGE)
    //             {
    //                 // TODO handle error better
    //                 // this is a message
    //                 roomNum = getroomNumber(packet);
    //                 if ((tempRoom = lookUpRoom(allRoom, roomNum)) != NULL)
    //                 {
    //                     serverRoomSpreadMessage(userList->userList[i].socket, tempRoom, packet);
    //                 }
    //             }

    //             else if (getpacketType(packet) == ISCOMM)
    //             {
    //                 char comType = getCommandType(packet);
    //                 if (comType == ROID)
    //                 {
    //                     switch (getCommandID(packet))
    //                     {
    //                     case ROCREATE:
    //                         if ((tempRoom = serverRoomCreate(allRoom)) != NULL)
    //                         {
    //                             // TODO: Check for if room is full
    //                             tempRoom->isOccupied = ROOM_OCCUPIED;
    //                             tempRoom->socketList[tempRoom->peopleNum - 1] = userList->userList[i].socket;
    //                             tempRoom->peopleNum += 1;
    //                         }
    //                         break;
    //                     case RODEL:
    //                         break;
    //                     }
    //                 }
    //             }
    //             else
    //             {
    //                 // not a recognizable format
    //             }
    //         }
    //     }
    //     // no data
    //     return -1;
    // }

#ifdef TEST_SERVER
int main(int argc, char *argv[])
{
    struct sockaddr *addr;
    socklen_t len;
    char dummyPacket[PACKAGE_SIZE];
    char receivedPacket[PACKAGE_SIZE];
    int socket = listenSocketInit();
    int newsocket = accept(socket, addr, &len);
    assert(newsocket >= 0);
    if (strcmp(argv[1], "TestClientSend") == 0)
    {
        recv(newsocket, receivedPacket, PACKAGE_SIZE, 0);
        printf("the comparision result is %d\n", strcmp(receivedPacket, testString1));
    }
    else if (strcmp(argv[1], "TestClientFetch") == 0)
    {
        assert(send(newsocket, testString1, PACKAGE_SIZE * sizeof(char), 0) >= 0);
        printf("\nsent package\n");
    }
    close(socket);
    close(newsocket);
    return 0;
}
#endif

// #ifdef TEST_SERVER
// int main(int argc, char *argv[])
// {
//     struct sockaddr *addr;
//     socklen_t len;
//     char dummyPacket[PACKAGE_SIZE];
//     char receivedPacket[PACKAGE_SIZE];
//     int socket = listenSocketInit();
//     int newsocket = accept(socket, addr, &len);
//     assert(newsocket >= 0);
//     if (strcmp(argv[1], "TestClientSend") == 0)
//     {
//         recv(newsocket, receivedPacket, PACKAGE_SIZE, 0);
//         printf("the comparision result is %d\n", strcmp(receivedPacket, testString1));
//     }
//     else if (strcmp(argv[1], "TestClientFetch") == 0)
//     {
//         assert(send(newsocket, testString1, PACKAGE_SIZE * sizeof(char), 0) >= 0);
//         printf("\nsent package\n");
//     }
//     close(socket);
//     close(newsocket);
//     return 0;
// }
// #endif